#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "word_db_master.hpp"


class ZhiZiWord {
  //typedef std::tuple<std::string, int, int, double, double, int> WordElement;
  typedef std::tuple<std::string, int, int, double, double> WordElement;

 public:
  ZhiZiWord(const string &word_db,const string &phrase_db);
  ~ZhiZiWord();
  void learn(const vector<string> &word, const string &text,const string &lang);
  json cut(const vector<string> &word, const string &text,const string &lang);
  json cutSpace(const vector<string> &word, const string &text,const string &lang);
  bool loadMaster(bool castForce = true);
  void unloadMaster(void);
  void commitArticle(void);
  void commitArticle(const json &task,const string &ws);
  void dumpDot(void);

 private:
  void getRawRank(const vector<string> &Bytes,const string &lang);
  void getOutRank(const string &text);
  void adjustRank(void);
  double adjustWeight(int width, double weight);
  void getNoConflictSeq(void);
  void dumpClearSeq(void);

  void calcPrediction(void);
  void calcPrediction(const multimap<int, WordElement> &confuse);
  json summaryCut(const string &text);
  string createGraph(const string &text,const string &sentence);
  
  vector<string> createAudioList(const string &lang);
  vector<string> createPhoneme(const string &word,const string &lang);
  
  void calcPredictionPhrase(const string &lang);
  void calcPredictionPhrase(const multimap<int, WordElement> &confuse,const string &lang);
  json summaryCutPhrase(const string &text,const string &lang);
  string createGraphPhrase(const string &text,const string &sentence,const string &lang);
 
  void collectWord(void);

  
  
  void dumpRank();
  void dumpSeq();
  void dumpPreds();
  void dumpPredWords();

 private:
  string database_word_;
  string database_phrase_;
  MasterDBWord dictInputCN_;
  MasterDBWord dictInputJA_;
  MasterDBWord phraseInputCN_;
  MasterDBWord phraseInputJA_;

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
