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

class OstrichWord {
public:
  OstrichWord(const string &database);
  ~OstrichWord();
  void learn(const vector<string> &word);
  void mergeWordByArticle(void);

private:
  void pushMultiWord(const string &word);  
private:
  DictionaryStorage dict_;
  map<string,int> multiWordOfOneArticle_;
  const int gWordLength = 32;
};
