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

class OstrichWord {
 public:
  OstrichWord(const string &database);
  OstrichWord();
  ~OstrichWord();
  void learn(const vector<string> &word);
  void commitArticle(const pt::ptree &task,const string &ws);
  void commitArticleIpfs(const pt::ptree &task,const string &ws);

 private:
  void pushMultiWord(const string &word);
  vector<string> pickupWordRanking(void);

 private:
  map<string, int> multiWordOfOneArticle_;
  const int maxWordLength_ = 32;
  const int minWordRepeateTimes_ = 9;
};
