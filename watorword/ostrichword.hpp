#include <string>
#include <iostream>
#include <tuple>
#include <vector>
#include <map>
#include <memory>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;

#include "dictstorage.hpp"

class OstrichWord {
public:
  OstrichWord(const string &database);
  OstrichWord();
  ~OstrichWord();
  void learn(const vector<string> &word);
  void mergeWordByArticle(void);
  void upWordByArticle2Master(const pt::ptree &task);

private:
  void pushMultiWord(const string &word);  
  string pickupWordRanking(void);
private:
  bool stub_;
  unique_ptr<DictionaryStorage> dict_;
  map<string,int> multiWordOfOneArticle_;
  const int gWordLength = 32;
  const int minWordRepeateTimes_ = 3;
};
