#include <string>
#include <iostream>
#include <tuple>
#include <vector>
#include <map>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "dictstorage.hpp"

class ParrotWord {
  typedef std::tuple<std::string,int,int,double,double> WordElement;
public:
  ParrotWord(const string &database);
  ~ParrotWord();
  void learn(const vector<string> &word,const string &text);
  bool loadMaster(bool forceCast = true);
  void unloadMaster(void);
  void commitArticle(void);
private:
  void getRawRank(const vector<string> &Bytes);
  void adjustRank(void);
  void cutTextByRank(const string &text);
  void calcPrediction(void);
  void getWordPrediction(const string &text);
  
  void mergeWordPrediction(void);
  
  void push2DB(void);


  
  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();
private:
  DictionaryStorage dict_;
  string database_;
  map<string,tuple<double,double>> statisticsRank_;
  multimap<int,WordElement> wordSeq_;
  multimap<int,WordElement> wordSeqTopSelected_;
  vector<string> prediWords_;
  int statisticsMinWordSize_;
  
  static map<string,int> gMultiWordSum;
  const int gWordLength = 32;
  leveldb::WriteBatch gSaveDBBatch;
};
