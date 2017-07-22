#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>


#include "charcut.hpp"

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define TRACE_VAR(x)

map<string,int> CharCut::gMultiWordSum;
//map<int,vector<string>> CharCut::gMultiWordRank;
map<string,int> CharCut::gOneWordSum;
//map<int,vector<string>> CharCut::gOneWordRank;


CharCut::CharCut(){
  openDB();  
}
CharCut::~CharCut() {
  closeDB();
}
bool CharCut::isCtrlMultiByte(const string &utf8) {
  TRACE_VAR(utf8.size());
  TRACE_VAR(utf8);
  auto it=gCtrlMultiByteWord.find(utf8);
  if(it!=gCtrlMultiByteWord.end()){
    return it->second;
  }
  return false;
}

bool CharCut::isOneByte(const string &utf8) {
  if(utf8.size() == 1){
    return true;
  }
  return false;
}

bool CharCut::isFiltOut(const string &utf8) {
  if(isCtrlMultiByte(utf8)){
    return true;
  }
  if(isOneByte(utf8)){
    return true;
  }
  return false;
}

void CharCut::pushMultiWord(const string &word,map<string,int> &wordSum){
  auto it = wordSum.find(word);
  if(it != wordSum.end()){
    it->second++;
  } else {
    wordSum[word]=1 ;
  }
}



void CharCut::pushOneWord(const string &word){
  auto it = gOneWordSum.find(word);
  if(it != gOneWordSum.end()){
    it->second++;
  } else {
    gOneWordSum[word]=1 ;
  }
}

void CharCut::pushContent(const vector<string> &input ){
  map<string,int> multiWordSum;
  list<string> preWords;
  for(auto &word:input) {
    if(isFiltOut(word)){
      TRACE_VAR(word);
      preWords.clear();
      continue;
    }
    /*one word*/
    pushOneWord(word);
    /* multi */
    auto it = preWords.rbegin();
    string jointWord(word);
    while(it!= preWords.rend()){
      jointWord = *it + jointWord;
      it++;
      pushMultiWord(jointWord,multiWordSum);
    }
    preWords.push_back(word);
    if(preWords.size() > gWordLength -1) {
      preWords.pop_front();
    }
  }
  mergeContentWord(multiWordSum);
}
void CharCut::mergeContentWord(const map<string,int> &wordSumMap) {
  DUMP_VAR(wordSumMap.size());
  map<int,vector<string>> localMultiWordRank;
  for(auto &wordSum:wordSumMap) {
    if(wordSum.second > 2) {
      TRACE_VAR(wordSum.first);
      TRACE_VAR(wordSum.second);
      auto it = localMultiWordRank.find(wordSum.second);
      if(it != localMultiWordRank.end()){
        it->second.push_back(wordSum.first);
      } else {
        localMultiWordRank[wordSum.second] ={wordSum.first};
      }
    }
  }
  TRACE_VAR(localMultiWordRank.size());  
  for(auto &record:localMultiWordRank) {
    DUMP_VAR(record.first);
    auto words = record.second;
    for(auto word:words){
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
       pushMultiWordGlobal(word,record.first); 
      }
    }
  }
  writeDB();
}

vector<string> CharCut::compressWordList(const vector<string> &words){
  vector<string> compressed;
  for(const auto &word:words){
    bool isShort= false;
    for(const auto &word2:words){
      auto found = word2.find(word);
      if(found != string::npos && word2 != word){
        isShort= true;
      }
    }
    if(isShort == false) {
      compressed.push_back(word);
    }
  }
  return compressed;
}


bool CharCut::matchWith(const string &word,vector<string> &words){
  auto it = words.begin();
  while(it != words.end()){
    auto found = word.find(*it);
    if(found != string::npos){
      it = words.erase(it);
    } else {
      it++;
    }
  }
  return false;
}

void CharCut::dumpSum(void){
}

#if 0
void CharCut::pushContent2(const vector<string> &input ){
  string preWord;
  string pre2Word;
  for(auto &word:input) {
    if(isFiltOut(word)){
      TRACE_VAR(word);
      preWord = "";
      pre2Word = "";
      continue;
    }
    if(preWord.empty() == false){
      pushMultiWord(preWord + word);
      if(pre2Word.empty() == false){
        pushMultiWord(pre2Word + preWord + word);
      }
    }
    pre2Word = preWord;
    preWord = word;
  }
}
#endif


