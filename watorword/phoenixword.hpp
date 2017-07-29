#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "dictstorage.hpp"
class PredictionTree;

class PhoenixWord {
  typedef std::tuple<std::string, int, int, double, double, int> WordElement;

public:
  PhoenixWord(const string &database);
  ~PhoenixWord();
  void learn(const vector<string> &word, const string &text);
  bool loadMaster(bool castForce = true);
  void unloadMaster(void);
  void commitArticle(void);

private:
  void getRawRank(const vector<string> &Bytes);
  void adjustRank(void);
  void cutTextByRank(const string &text);

  void getNoConflictSeq(void);
  void dumpClearSeq(void);

  void calcPrediction(void);
  void calcPrediction(const multimap<int, WordElement> &clearWordSeq);
  void getWordPrediction(const string &text);

  void mergeWordPrediction(void);

  void push2DB(void);
  void fetchPrediction(shared_ptr<PredictionTree> predTree);

  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();

private:
  DictionaryStorage dict_;
  string database_;

  map<string, tuple<double, double>> phoenixRank_;
  int rawRankMinWordSize_;

  multimap<int, WordElement> wordSeq_;
  vector<multimap<int, WordElement>> noConflictWordSeq_;
  multimap<int, WordElement> wordSeqTopSelected_;
  vector<string> prediWords_;

  static map<string, int> gMultiWordSum;
  const int gWordLength = 32;
  leveldb::WriteBatch gSaveDBBatch;
};
