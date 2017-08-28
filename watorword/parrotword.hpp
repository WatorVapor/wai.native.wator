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


class ParrotWord {
  typedef std::tuple<std::string, int, int, double, double> WordElement;

 public:
  ParrotWord(const string &database);
  ~ParrotWord();
  void learn(const vector<string> &word, const string &text,const string &lang);
  bool loadMaster(bool forceCast = true);
  void unloadMaster(void);
  void commitArticle(void);
  void commitArticle(const pt::ptree &task);

 private:
  void getRawRank(const vector<string> &Bytes);
  void adjustRank(void);
  double adjustWeight(int width, double weight);
  void cutTextByRank(const string &text);
  void calcPrediction(void);
  void getWordPrediction(const string &text);
  vector<string> pickupWordRanking(void);

  void collectWord(void);

  void dumpDot(void);

  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();

 private:
  string database_;
  MasterDBWord dictInputCN_;
  MasterDBWord dictInputJA_;
  map<string, tuple<double, double>> statisticsRank_;
  multimap<int, WordElement> wordHintSeq_;
  multimap<int, WordElement> wordSeqTopSelected_;
  vector<string> prediWords_;
  int statisticsMinWordSize_;
  int ap_ = 0;

  static map<string, int> multiWordOfOneArticle_;
  const int gWordLength = 32;
  const int minWordRepeateTimes_ = 9;
};
