#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

class ParrotWord {
  typedef std::tuple<std::string, int, int, double, double> WordElement;

 public:
  ParrotWord(const string &database);
  ~ParrotWord();
  void learn(const vector<string> &word, const string &text);
  bool loadMaster(bool forceCast = true);
  void unloadMaster(void);
  void commitArticle(void);
  void commitArticle(const pt::ptree &task);

 private:
  void getRawRank(const vector<string> &Bytes);
  void adjustRank(void);
  double adjustWeight(int width,double weight);
  void cutTextByRank(const string &text);
  void calcPrediction(void);
  void getWordPrediction(const string &text);

  void mergeWordPrediction(void);
  
  void dumpDot(void);

  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();

 private:
  string database_;
  map<string, tuple<double, double>> statisticsRank_;
  multimap<int, WordElement> wordHintSeq_;
  multimap<int, WordElement> wordSeq_;
  multimap<int, WordElement> wordSeqTopSelected_;
  vector<string> prediWords_;
  int statisticsMinWordSize_;
  int ap_ = 0;

  static map<string, int> gMultiWordSum;
  const int gWordLength = 32;
  const int minWordRepeateTimes_ = 9;
};
