#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include "ctrlclaw.hpp"
#include "log.hpp"

CtrlClaw::CtrlClaw() {}
CtrlClaw::~CtrlClaw() {}
bool CtrlClaw::isCtrlMultiByte(const string &utf8) {
  TRACE_VAR(utf8.size());
  TRACE_VAR(utf8);
  auto it = gCtrlMultiByteWord.find(utf8);
  if (it != gCtrlMultiByteWord.end()) {
    return it->second;
  }
  return false;
}

bool CtrlClaw::isOneByte(const string &utf8) {
  if (utf8.size() == 1) {
    return true;
  }
  return false;
}
bool CtrlClaw::isTwoByte(const string &utf8) {
  if (utf8.size() == 2) {
    return true;
  }
  return false;
}

void parseUTF8(const string &article, vector<string> &words,
               vector<int> &positions);

void CtrlClaw::cut(const string &article) {
  this->clear();
  article_ = article;
  parseUTF8(article, words_, positions_);
}

void CtrlClaw::claw(const string &article) {
  this->clear();
  article_ = article;
  parseUTF8(article, words_, positions_);
  TRACE_VAR(words_.size(), positions_.size());
  int startPos = 0;
  auto startIt = words_.begin();
  auto nowIt = startIt;
  bool otherFlag = false;
  string otherWord;
  vector<string> otherWordVect;
  for (int i = 0; i < words_.size() && i < positions_.size(); i++) {
    auto mbyte = words_[i];
    TRACE_VAR(mbyte);
    if (this->isOneByte(mbyte) || this->isTwoByte(mbyte) ||
        this->isCtrlMultiByte(mbyte)) {
      // auto size = positions_[i] - startPos + mbyte.size();
      auto size = positions_[i] - startPos;
      TRACE_VAR(size);
      if (size > 0) {
        auto sub = article.substr(startPos, size);
        sentences_.push_back(sub);
        TRACE_VAR(sub);
        TRACE_VAR(*startIt);
        TRACE_VAR(*nowIt);
        vector<string> subwords(startIt, nowIt);
        TRACE_VAR(std::distance(startIt, nowIt));
        auto sword = std::make_tuple(sub, subwords,true);
        sentence_words_.push_back(sword);
        TRACE_VAR(sentence_words_.size());
      }
      startPos = positions_[i] + mbyte.size();
      nowIt++;
      startIt = nowIt;
      otherFlag = false;
    } else {
      DUMP_VAR(mbyte);
      nowIt++;
      otherWord += mbyte;
      otherWordVect.push_back(mbyte);
      if(otherFlag == false) {
        auto sword = std::make_tuple(otherWord, otherWordVect,false);
        sentence_words_.push_back(sword);
        otherWord.clear();
        otherWordVect.clear();
      }
      otherFlag = true;
    }
  }
  if (startPos < article.size()) {
    string sub;
    vector<string> subwords;
    for (; startIt != words_.end(); startIt++) {
      if (this->isOneByte(*startIt) || this->isTwoByte(*startIt) ||
          this->isCtrlMultiByte(*startIt)) {
        continue;
      }
      subwords.push_back(*startIt);
      sub += *startIt;
    }
    TRACE_VAR(sub);
    sentences_.push_back(sub);
    auto sword = std::make_tuple(sub, subwords,true);
    sentence_words_.push_back(sword);
  }
  if(!otherWord.empty() && !otherWordVect.empty()) {
    auto sword = std::make_tuple(otherWord, otherWordVect,false);
    sentence_words_.push_back(sword);
  }
}
void CtrlClaw::clear(void) {
  article_.clear();
  sentences_.clear();
  sentence_words_.clear();
  words_.clear();
  positions_.clear();
}
