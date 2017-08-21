#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

class PredictionTree;
class MasterDBWord;

class QiangbaoWord {
  typedef std::tuple<std::string, int, int, double, double, int> WordElement;

 public:
  QiangbaoWord(const string &database);
  ~QiangbaoWord();
  void learn(const vector<string> &word, const string &text);
  pt::ptree cut(const vector<string> &word, const string &text, double &pred);
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
  string database_;
  pt::ptree result_;
  string clearResult_;
  shared_ptr<MasterDBWord> master_;

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
