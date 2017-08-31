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

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "log.hpp"
#include "phoenixword.hpp"

double getDoublePred(const string &word);

map<string, int> PhoenixWord::gMultiWordSum;

static int iConstWordBatchMax = 1000;
static const int gWeightAdjustBase = 4;
static const double gWeightAdjustRateMax = 16.0;

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
  //this->adjustRank();
  //  this->dumpRank();

  //this->cutTextByRank(text);
  //  this->dumpSeq();

  //this->getNoConflictSeq();
  //  this->dumpClearSeq();

  //this->calcPrediction();
}

void PhoenixWord::mergeWordPrediction(void) {
  for (auto phoenix : prediWords_) {
    auto it = gMultiWordSum.find(phoenix);
    if (it != gMultiWordSum.end()) {
      it->second++;
    } else {
      gMultiWordSum[phoenix] = 1;
    }
  }
}

void PhoenixWord::commitArticle(void) {
  this->push2DB();
  gMultiWordSum.clear();
}

void PhoenixWord::getRawRank(const vector<string> &Bytes,const string &lang) {
  statisticsMinWordSize_ = 32;
  wordHintSeq_.clear();
  wordSeqTopSelected_.clear();
  prediWords_.clear();
  ap_++;

  list<string> preWords;
  TRACE_VAR(statisticsMinWordSize_);
  int baseLinePos = 0;
  for (auto &word : Bytes) {
    /* multi */
    TRACE_VAR(word);
    auto it = preWords.rbegin();
    string jointWord(word);
    int wordPos = baseLinePos;
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



void PhoenixWord::adjustRank() {
  for (auto rPair : phoenixRank_) {
    TRACE_VAR(rPair.first, rPair.second);
    auto word = rPair.first;
    auto weight = std::get<0>(rPair.second);
    TRACE_VAR(word, weight, word.size());
    double rate = (double)word.size() / (double)rawRankMinWordSize_ - 1.0;
    double rate2 = ::pow(gWeightAdjustBase, rate);
    if (rate > gWeightAdjustRateMax) {
      DUMP_VAR4(word, word.size(), rawRankMinWordSize_, rate);
      rate2 = 1.0;
    }
    TRACE_VAR(rate, rate2);
    auto weight_adj = weight * (int)rate2;
    TRACE_VAR(word, word.size(), weight, weight_adj);
    phoenixRank_[word] = std::make_tuple(weight_adj, weight);
    ;
  }
}

static vector<size_t> find_all_substr(const string &sub, const string &text) {
  vector<size_t> positions;
  auto pos = text.find(sub, 0);
  while (pos != string::npos) {
    positions.push_back(pos);
    pos = text.find(sub, pos + 1);
  }
  return positions;
}

/*
void PhoenixWord::cutTextByRank(const string &text) {
  for (auto it = phoenixRank_.begin(); it != phoenixRank_.end(); it++) {
    auto rankRepeat = std::get<0>(it->second);
    auto rankorig = std::get<1>(it->second);
    TRACE_VAR(rankRepeat);
    auto word = it->first;
    TRACE_VAR(word);
    auto posAll = find_all_substr(word, text);
    for (auto pos : posAll) {
      auto range = word.size();
      TRACE_VAR(word, pos, range, rankRepeat, rankorig);
      auto elem =
          std::make_tuple(word, pos, word.size(), rankRepeat, rankorig, 0);
      wordSeq_.insert(std::make_pair(pos, elem));
    }
  }
}
*/

#include <boost/algorithm/string.hpp>

void PhoenixWord::getWordPrediction(const string &text) {
  string textRemain(text);
  for (auto elem : wordSeqTopSelected_) {
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word, pos, range, weight);
    boost::algorithm::replace_all(textRemain, word, " ");
    prediWords_.push_back(word);
  }
  DUMP_VAR(textRemain);
  list<string> list_textRemain;
  boost::split(list_textRemain, textRemain, boost::is_space(),
               boost::algorithm::token_compress_on);
  for (auto remain : list_textRemain) {
    TRACE_VAR(remain);
    TRACE_VAR(remain.size());
    if (remain.size() > 1 && remain.size() < 4) {
      prediWords_.push_back(remain);
    }
  }
}

void PhoenixWord::getNoConflictSeq(void) {
  multimap<int, WordElement> nocfWordSeq;
  int maxPreCover = 0;
  for (auto it = wordSeq_.begin(); it != wordSeq_.end(); it++) {
    if (wordSeq_.begin() == it) {
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
  this->mergeWordPrediction();
}

#include <typeinfo>
#include "treeword.hpp"

#if 0
void PhoenixWord::calcPrediction(
    const multimap<int, WordElement> &clearWordSeq) {
  auto predTree = std::make_shared<PredictionTree>();
  // DUMP_VAR(typeid(predTree).name());
  bool seqResult = predTree->setWordSeq(clearWordSeq);
  if (seqResult) {
    fetchPrediction(predTree);
  } else {
    vector<multimap<int, WordElement>> splitedSeq;
    predTree->splitComplexWordSeq(clearWordSeq, splitedSeq);
    if (splitedSeq.size() > 1) {
      int i = 0;
      for (auto seq : splitedSeq) {
        DUMP_VAR2(i++, splitedSeq.size());
        for (auto elem : seq) {
          auto word = std::get<0>(elem.second);
          auto pos = std::get<1>(elem.second);
          auto range = std::get<2>(elem.second);
          auto weight = std::get<3>(elem.second);
          auto weight_orig = std::get<4>(elem.second);
          DUMP_VAR5(word, pos, range, weight, weight_orig);
        }
        calcPrediction(seq);
      }
    }
  }
}


void PhoenixWord::fetchPrediction(shared_ptr<PredictionTree> predTree) {
  // predTree->dumpAllPreds();
  multimap<int, vector<WordElement>> flat;
  double weightSum = 0.0;
  predTree->flatPredSeq(flat, weightSum);

  TRACE_VAR(weightSum);
  auto top = flat.rbegin();
  int maxTop = 1;
  for (; top != flat.rend(); top++) {
    std::cout << "-------------" << std::endl;
    auto fredFloat = (double)(top->first) / (double)(weightSum);
    DUMP_VAR2(weightSum, fredFloat);
    if (--maxTop < 0) {
      break;
    }
    for (auto elem : top->second) {
      auto word = std::get<0>(elem);
      auto pos = std::get<1>(elem);
      auto range = std::get<2>(elem);
      auto weight = std::get<3>(elem);
      auto weight_orig = std::get<4>(elem);
      DUMP_VAR5(word, pos, range, weight, weight_orig);
      prediWords_.push_back(word);
    }
  }
}
#endif 
