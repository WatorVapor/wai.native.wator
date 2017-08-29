#include <algorithm>
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

#include "parrotword.hpp"

#include "log.hpp"

static int iConstWordBatchMax = 1000;

ParrotWord::ParrotWord(const string &database)
  : database_(database)
  ,dictInputCN_()
  ,dictInputJA_() {
  }
ParrotWord::~ParrotWord() {
}

void ParrotWord::learn(const vector<string> &wordBytes, const string &text,const string &lang) {
  if (wordBytes.size() < 2) {
    return;
  }
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  //this->dumpDot();
  // this->adjustRank();
  // this->dumpRank();
  // this->cutTextByRank(text);
  // this->dumpSeq();
  this->calcPrediction();
  this->getWordPrediction(text);
  TRACE_VAR(text);
  // this->dumpPreds();
  //this->dumpPredWords();
  this->collectWord();
}
void ParrotWord::dumpPredWords() {
  for (auto parrot : prediWords_) {
  std:
    cout << "'" << parrot << "',";
  }
  std::cout << std::endl;
}

void ParrotWord::getRawRank(const vector<string> &Bytes,const string &lang) {
  statisticsRank_.clear();
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

static const int gWeigthAdjustBase = 256;

double ParrotWord::adjustWeight(int width, double weight) {
  double rate = (double)width / (double)statisticsMinWordSize_ - 1.0;
  double rate2 = ::pow(gWeigthAdjustBase, rate);
  TRACE_VAR(rate, rate2);
  auto weight_adj = weight * rate2;
  return weight_adj;
}

void ParrotWord::calcPrediction(void) {
  TRACE_VAR(wordHintSeq_.size());
  if (wordHintSeq_.empty()) {
    return;
  }
  multimap<double, WordElement> weightElem;
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
    weightElem.insert(std::make_pair(weight_adj, elemNew));
  }
  auto lastPos = wordHintSeq_.rbegin();
  auto firstPos = wordHintSeq_.begin();
  auto place_length = lastPos->first - firstPos->first;
  TRACE_VAR(lastPos->first, firstPos->first);

  place_length += std::get<2>(lastPos->second);
  TRACE_VAR(place_length);
  vector<bool> placeholder;
  for (int i = 0; i < place_length + 1; i++) {
    placeholder.push_back(false);
  }

  // std::cout << "---------------" <<std::endl;
  auto place_begin = firstPos->first;

  for (auto itWeight = weightElem.rbegin(); itWeight != weightElem.rend();
       itWeight++) {
    TRACE_VAR(itWeight->first);
    auto word = std::get<0>(itWeight->second);
    auto pos = std::get<1>(itWeight->second);
    auto range = std::get<2>(itWeight->second);
    auto weight = std::get<3>(itWeight->second);
    TRACE_VAR(word, pos, range, weight);

    auto offsetWord = pos - place_begin;
    bool hasPlaceFlag = true;
    for (int i = offsetWord; i < offsetWord + range; i++) {
      if (i >= placeholder.size()) {
        continue;
      }
      if (placeholder.at(i)) {
        hasPlaceFlag = false;
        break;
      } else {
        placeholder[i] = true;
      }
    }
    TRACE_VAR(hasPlaceFlag);
    if (hasPlaceFlag) {
      wordSeqTopSelected_.insert(std::make_pair(pos, itWeight->second));
    }
  }
  TRACE_VAR(wordSeqTopSelected_.size());
}

#include <boost/algorithm/string.hpp>

void ParrotWord::getWordPrediction(const string &text) {
  string textRemain(text);
  for (auto elem : wordSeqTopSelected_) {
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    TRACE_VAR(word, pos, range, weight);
    boost::algorithm::replace_all(textRemain, word, " ");
    prediWords_.push_back(word);
  }
  TRACE_VAR(textRemain);
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
