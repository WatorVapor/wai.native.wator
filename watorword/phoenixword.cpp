#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;


#include "log.hpp"
#include "phoenixword.hpp"



PhoenixWord::PhoenixWord(const string &database) : database_(database) {}
PhoenixWord::~PhoenixWord() {}
void PhoenixWord::learn(const vector<string> &wordBytes, const string &text,const string &lang) {
  if (wordBytes.size() < 2) {
    return;
  }
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  //this->dumpDot();
  this->adjustRank();
  //  this->dumpRank();


  this->getNoConflictSeq();
  //this->dumpClearSeq();

  this->calcPrediction();
  
  this->collectWord();
}

pt::ptree PhoenixWord::cut(const vector<string> &wordBytes, const string &text,
                            const string &lang) {
  pt::ptree result;
  if (wordBytes.size() < 2) {
    return result;
  }
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  //this->dumpDot();
  this->adjustRank();
  //  this->dumpRank();


  this->getNoConflictSeq();

  this->calcPrediction();
  return this->summaryCut();
}

pt::ptree PhoenixWord::summaryCut(void) {
  pt::ptree result;
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
  }
  //result.put(u8"sentence", clearResult_);
  //result.add_child(u8"detail", result_);
  return result;
}



void PhoenixWord::commitArticle(void) {
  multiWordOfOneArticle_.clear();
}

void PhoenixWord::getRawRank(const vector<string> &Bytes,const string &lang) {
  statisticsMinWordSize_ = 32;
  wordHintSeq_.clear();
  wordAdjustedSeq_.clear();
  statisticsRank_.clear();
  noConflictWordSeq_.clear();
  wordSeqTopSelected_.clear();
  ap_++;

  list<string> preWords;
  TRACE_VAR(statisticsMinWordSize_);
  int baseLinePos = 0;
  for (auto &word : Bytes) {
    TRACE_VAR(word);
    int wordPos = baseLinePos;
    /* single */
    {
      auto pred = -1.0;
      if(lang =="cn") {
        pred = dictInputCN_.getDoublePred(word);
      }
      if(lang =="ja") {
        pred = dictInputJA_.getDoublePred(word);
      }
      if (pred > 0) {
        TRACE_VAR(word, pred);
        TRACE_VAR(word.size());
        if (statisticsMinWordSize_ > word.size()) {
          statisticsMinWordSize_ = word.size();
        }
        statisticsRank_[word] = std::make_tuple(pred, pred);
        TRACE_VAR(word, wordPos, pred);
        auto elem =
            std::make_tuple(word, wordPos, word.size(), pred, pred);
        wordHintSeq_.insert(std::make_pair(wordPos, elem));
      }      
    }
    /* multi */
    auto it = preWords.rbegin();
    string jointWord(word);
    while (it != preWords.rend()) {
      jointWord = *it + jointWord;
      TRACE_VAR(jointWord);
      auto pred = -1.0;
      if(lang =="cn") {
        pred = dictInputCN_.getDoublePred(jointWord);
      }
      if(lang =="ja") {
        pred = dictInputJA_.getDoublePred(jointWord);
      }
      wordPos -= it->size();
      if (pred > 0) {
        TRACE_VAR(jointWord, pred);
        TRACE_VAR(jointWord.size());
        if (statisticsMinWordSize_ > jointWord.size()) {
          statisticsMinWordSize_ = jointWord.size();
        }
        statisticsRank_[jointWord] = std::make_tuple(pred, pred);

        TRACE_VAR(jointWord, wordPos, pred);
        auto elem =
            std::make_tuple(jointWord, wordPos, jointWord.size(), pred, pred);
        wordHintSeq_.insert(std::make_pair(wordPos, elem));
      }
      it++;
    }
    baseLinePos += word.size();
    preWords.push_back(word);
    if (preWords.size() > gWordLength - 1) {
      preWords.pop_front();
    }
  }
  TRACE_VAR(statisticsMinWordSize_);
}

static const double gWeigthAdjustBase = 4096.0;

double PhoenixWord::adjustWeight(int width, double weight) {
  double rate = (double)width / (double)statisticsMinWordSize_ - 1.0;
  double rate2 = ::pow(gWeigthAdjustBase, rate);
  TRACE_VAR(rate, rate2);
  auto weight_adj = weight * rate2;
  return weight_adj;
}


void PhoenixWord::adjustRank() {
  for (auto elem : wordHintSeq_) {
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    auto weight2 = std::get<4>(elem.second);
    auto weight_adj = adjustWeight(word.size(), weight);
    TRACE_VAR(word, pos, range, weight_adj, weight);
    auto elemNew = std::make_tuple(word, pos, range, weight_adj, weight2);
    wordAdjustedSeq_.insert(std::make_pair(pos, elemNew));
  }
}




void PhoenixWord::getNoConflictSeq(void) {
  multimap<int, WordElement> nocfWordSeq;
  int maxPreCover = 0;
  for (auto it = wordAdjustedSeq_.begin(); it != wordAdjustedSeq_.end(); it++) {
    if (wordAdjustedSeq_.begin() == it) {
      nocfWordSeq.insert(std::make_pair(it->first, it->second));
      continue;
    }
    auto prevIt = std::prev(it);
    auto wordPre = std::get<0>(prevIt->second);
    auto posPre = std::get<1>(prevIt->second);
    auto rangePre = std::get<2>(prevIt->second);
    auto preCover = posPre + rangePre;
    if (maxPreCover < preCover) {
      maxPreCover = preCover;
    }
    auto curWord = std::get<0>(it->second);
    auto curPos = std::get<1>(it->second);
    if (maxPreCover <= curPos) {
      TRACE_VAR(curWord, curPos, true);
      noConflictWordSeq_.push_back(nocfWordSeq);
      nocfWordSeq.clear();
      maxPreCover = 0;
    } else {
      TRACE_VAR(curWord, curPos, false);
    }
    nocfWordSeq.insert(std::make_pair(it->first, it->second));
  }
  TRACE_VAR(noConflictWordSeq_.size());
  if (nocfWordSeq.empty() == false) {
    noConflictWordSeq_.push_back(nocfWordSeq);
  }
}

void PhoenixWord::calcPrediction(void) {
  for (auto clearSeq : noConflictWordSeq_) {
    // std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    /*
        for(auto elem :clearSeq){
         auto word = std::get<0>(elem.second);
         auto pos = std::get<1>(elem.second);
         auto range = std::get<2>(elem.second);
         auto weight = std::get<3>(elem.second);
         auto weight_orig = std::get<4>(elem.second);
         DUMP_VAR5(word,pos,range,weight,weight_orig);
        }
    */
    this->calcPrediction(clearSeq);
    // std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
}



