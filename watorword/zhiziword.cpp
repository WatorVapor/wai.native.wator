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
#include "zhiziword.hpp"



ZhiZiWord::ZhiZiWord(const string &word_db,const string &phrase_db) : database_word_(word_db),database_phrase_(phrase_db) {}
ZhiZiWord::~ZhiZiWord() {}
void ZhiZiWord::learn(const vector<string> &wordBytes, const string &text,const string &lang) {
  if (wordBytes.size() < 2) {
    return;
  }
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  this->getOutRank(text);
  //this->dumpDot();
  this->adjustRank();
  //  this->dumpRank();


  this->getNoConflictSeq();
  //this->dumpClearSeq();

  this->calcPrediction();
  
  this->collectWord();
}

json ZhiZiWord::cut(const vector<string> &wordBytes, const string &text,
                            const string &lang) {
  json result;
/*  
  if (wordBytes.size() < 2) {
    return result;
  }
*/
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  this->getOutRank(text);
  //this->dumpDot();
  this->adjustRank();
  //  this->dumpRank();


  this->getNoConflictSeq();

  if(lang =="cn") {
    this->calcPredictionPhrase(lang);
    return this->summaryCutPhrase(text,lang);
  } else {
    this->calcPrediction();
    return this->summaryCut(text,lang);
  }
}




json ZhiZiWord::summaryCut(const string &text,const string &lang) {
  json result;
  string sentence = "";
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
    sentence += word + "%";
  }
  sentence.pop_back();
  result[u8"sentence"] = sentence;
  auto graph = this->createGraph(text,sentence);
  result[u8"graph"] = graph;
  result[u8"input"] = text;
  
  auto audioList = this->createAudioList(lang);
  json audioPt;
  for(auto audio:audioList) {
    audioPt.push_back(audio);
  }
  result[u8"tts"] = audioPt;
  result[u8"lang"] = lang;

  return result;
}

json ZhiZiWord::summaryCutPhrase(const string &text,const string &lang) {
  json result;
  string sentence = "";
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
    sentence += word + "%";
  }
  sentence.pop_back();
  result[u8"sentence"] = sentence;
  auto graph = this->createGraphPhrase(text,sentence,lang);
  result[u8"graph"] = graph;
  result[u8"input"] = text;
  auto audioList = this->createAudioList(lang);
  json audioPt;
  for(auto audio:audioList) {
    audioPt.push_back(audio);
  }
  result[u8"tts"] = audioPt;
  result[u8"lang"] = lang;
  return result;
}




void ZhiZiWord::commitArticle(void) {
  multiWordOfOneArticle_.clear();
}

void ZhiZiWord::getRawRank(const vector<string> &Bytes,const string &lang) {
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

double dConstMissMatchPred = 0.00000001;

void ZhiZiWord::getOutRank(const string &text) {
  multimap<int, WordElement> wordMissSeq;
  for (auto it = wordHintSeq_.begin();it != wordHintSeq_.end();it++) {
    TRACE_VAR(it->first);
    auto word = std::get<0>(it->second);
    TRACE_VAR(it->first,word,word.size());
    auto itNext = it;
    itNext++;
    if(itNext != wordHintSeq_.end()) {
      auto coverNext = it->first + word.size();
      TRACE_VAR(coverNext,itNext->first);
      if(coverNext < itNext->first) {
        auto size = itNext->first - coverNext;
        auto wordOut = text.substr(coverNext,size);
        DUMP_VAR3(coverNext,itNext->first,wordOut);
        auto elem =
            std::make_tuple(wordOut, coverNext, wordOut.size(), dConstMissMatchPred, dConstMissMatchPred);
        wordMissSeq.insert(std::make_pair(coverNext, elem));
      }
    }
  }
  DUMP_VAR(wordMissSeq.size());
  DUMP_VAR(wordHintSeq_.size());
  for(auto miss:wordMissSeq) {
    wordHintSeq_.insert(miss);
  }
  DUMP_VAR(wordHintSeq_.size());
}

static const double gWeigthAdjustBase = 4096.0;

double ZhiZiWord::adjustWeight(int width, double weight) {
  double rate = (double)width / (double)statisticsMinWordSize_ - 1.0;
  double rate2 = ::pow(gWeigthAdjustBase, rate);
  TRACE_VAR(rate, rate2);
  auto weight_adj = weight * rate2;
  return weight_adj;
}


void ZhiZiWord::adjustRank() {
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




void ZhiZiWord::getNoConflictSeq(void) {
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

void ZhiZiWord::calcPrediction(void) {
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

void ZhiZiWord::calcPredictionPhrase(const string &lang) {
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
    this->calcPredictionPhrase(clearSeq,lang);
    // std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
}

#include <boost/algorithm/string.hpp>

json ZhiZiWord::cutSpace(const vector<string> &wordBytes, const string &text,
                            const string &lang) {
  json result;
  DUMP_VAR(text);
  string sentence = "";
  for(auto mbyte:wordBytes) {
    if(mbyte.size() == 1) {
      if(::isalnum (mbyte.at(0))) {
        sentence += mbyte;
      } else {
        sentence += mbyte;
        sentence += "%";
      }
    }
  }
  result[u8"sentence"] = sentence;
  result[u8"graph"] = "";
  result[u8"input"] = text;
  return result;
}


