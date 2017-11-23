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

//class PredictionTree;

class PhoenixWord {
  //typedef std::tuple<std::string, int, int, double, double, int> WordElement;
  typedef std::tuple<std::string, int, int, double, double> WordElement;

 public:
  PhoenixWord(const string &database);
  ~PhoenixWord();
  void learn(const vector<string> &word, const string &text,const string &lang);
  pt::ptree cut(const vector<string> &word, const string &text,const string &lang);
  pt::ptree cutSpace(const vector<string> &word, const string &text,const string &lang);
  bool loadMaster(bool castForce = true);
  void unloadMaster(void);
  void commitArticle(void);
  void commitArticle(const pt::ptree &task,const string &ws);
  void dumpDot(void);

 private:
  void getRawRank(const vector<string> &Bytes,const string &lang);
  void getOutRank(const string &text);
  void calcPair(void);
  
  void adjustRank(void);
  double adjustWeight(int width, double weight);
  void getNoConflictSeq(void);
  void dumpClearSeq(void);

  void calcPrediction(void);
  void calcPrediction(const multimap<int, WordElement> &confuse);
  pt::ptree summaryCut(const string &text);
  string createGraph(const string &text,const string &sentence);

 
  void collectWord(void);
  void pushMultiWord(const string &word);

  
  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();

 private:
  string database_;
  MasterDBWord dictInputCN_;
  MasterDBWord dictInputJA_;

  map<string, tuple<double, double>> statisticsRank_;
  int statisticsMinWordSize_;  
  int ap_;
  multimap<int, WordElement> wordHintSeq_;
  multimap<int, WordElement> wordAdjustedSeq_;
  vector<multimap<int, WordElement>> noConflictWordSeq_;
  multimap<int, WordElement> wordSeqTopSelected_;

  static map<string, int> multiWordOfOneArticle_;
  
  const int gWordLength = 32;
  const int minWordRepeateTimes_ = 5;
};
