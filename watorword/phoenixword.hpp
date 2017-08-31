#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

#include "word_db_master.hpp"

class PredictionTree;

class PhoenixWord {
  typedef std::tuple<std::string, int, int, double, double, int> WordElement;

 public:
  PhoenixWord(const string &database);
  ~PhoenixWord();
  void learn(const vector<string> &word, const string &text,const string &lang);
  bool loadMaster(bool castForce = true);
  void unloadMaster(void);
  void commitArticle(void);
  void commitArticle(const pt::ptree &task,const string &ws);
  void dumpDot(void);

 private:
  void getRawRank(const vector<string> &Bytes,const string &lang);
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
  MasterDBWord dictInputCN_;
  MasterDBWord dictInputJA_;

  map<string, tuple<double, double>> phoenixRank_;
  int rawRankMinWordSize_;
  int statisticsMinWordSize_;
  int ap_;

  multimap<int, WordElement> wordHintSeq_;
  multimap<int, WordElement> wordSeq_;
  vector<multimap<int, WordElement>> noConflictWordSeq_;
  multimap<int, WordElement> wordSeqTopSelected_;
  vector<string> prediWords_;

  static map<string, int> gMultiWordSum;
  const int gWordLength = 32;
  const int minWordRepeateTimes_ = 9;
};
