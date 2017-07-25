#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "ostrichword.hpp"

int getPred(const string &word);

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define DUMP_VAR2(x,y) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "=<" << x << "," << y << ">" << std::endl;
#define DUMP_VAR3(x,y,z) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "=<" << x << "," << y << "," << z <<">" << std::endl;
#define DUMP_VAR4(x,y,z,a) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "=<" << x << "," << y << "," << z << "," << a <<">" << std::endl;
#define DUMP_VAR5(x,y,z,a,b) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "," << #b << "=<" << x << "," << y << "," << z << "," << a << "," << b <<">" << std::endl;

#define TRACE_VAR(...)

static int iConstWordBatchMax = 1000;

OstrichWord::OstrichWord(const string &database) :
stub_(false) {
  dict_ = std::unique_ptr<DictionaryStorage>(new DictionaryStorage(database, "word_ostrich"));
  dict_->openDB();
}

OstrichWord::OstrichWord() :
stub_(true) {
}

OstrichWord::~OstrichWord() {
  if(stub_ == false){
    dict_->closeDB();
  }
}
void OstrichWord::learn(const vector<string> &wordBytes){
  if(wordBytes.size() <2) {
    return;
  }
  list<string> preWords;
  for(auto &word:wordBytes) {
    /* multi */
    auto it = preWords.rbegin();
    string jointWord(word);
    while(it!= preWords.rend()){
      jointWord = *it + jointWord;
      it++;
      this->pushMultiWord(jointWord);
    }
    preWords.push_back(word);
    if(preWords.size() > maxWordLength_ -1) {
      preWords.pop_front();
    }
  }
}


string OstrichWord::pickupWordRanking(void) {
  DUMP_VAR(multiWordOfOneArticle_.size());
  map<int,vector<string>> localMultiWordRank;
  for(auto wordSum:multiWordOfOneArticle_) {
    TRACE_VAR(wordSum.first,wordSum.second);
    if(wordSum.second >= minWordRepeateTimes_) {
      TRACE_VAR(wordSum.first,wordSum.second);
      auto it = localMultiWordRank.find(wordSum.second);
      if(it != localMultiWordRank.end()){
        it->second.push_back(wordSum.first);
      } else {
        localMultiWordRank[wordSum.second] ={wordSum.first};
      }
    }
  }
  DUMP_VAR(localMultiWordRank.size());
  string wordArrays;
  for(auto &record:localMultiWordRank) {
    TRACE_VAR(record.first);
    auto words = record.second;
    for(auto word:words) {
      bool isShort= false;
      for(const auto &word2:words){
        auto found = word2.find(word);
        if(found != string::npos && word2 != word){
          TRACE_VAR(word);
          TRACE_VAR(word2);
          isShort= true;
        }
      }
      if(isShort == false) {
        wordArrays += "{";
        wordArrays += word;
        wordArrays += ",";
        wordArrays += std::to_string(record.first);
        wordArrays += "};";
      }
    }
  }
  wordArrays += "{}";
  return wordArrays;
}

void OstrichWord::pushMultiWord(const string &word){
  auto it = multiWordOfOneArticle_.find(word);
  if(it != multiWordOfOneArticle_.end()){
    it->second++;
  } else {
    multiWordOfOneArticle_[word]=1 ;
  }
}

