#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/format.hpp>

#include "log.hpp"
#include "ostrichword.hpp"

void OstrichWord::mergeWordByArticle(void) {
  DUMP_VAR(multiWordOfOneArticle_.size());
  map<int, vector<string>> localMultiWordRank;
  for (auto wordSum : multiWordOfOneArticle_) {
    TRACE_VAR(wordSum.first, wordSum.second);
    if (wordSum.second > 2) {
      TRACE_VAR(wordSum.first, wordSum.second);
      auto it = localMultiWordRank.find(wordSum.second);
      if (it != localMultiWordRank.end()) {
        it->second.push_back(wordSum.first);
      } else {
        localMultiWordRank[wordSum.second] = {wordSum.first};
      }
    }
  }
  DUMP_VAR(localMultiWordRank.size());
  for (auto &record : localMultiWordRank) {
    TRACE_VAR(record.first);
    auto words = record.second;
    for (auto word : words) {
      bool isShort = false;
      for (const auto &word2 : words) {
        auto found = word2.find(word);
        if (found != string::npos && word2 != word) {
          TRACE_VAR(word);
          TRACE_VAR(word2);
          isShort = true;
        }
      }
      if (isShort == false) {
        if (stub_) {
        } else {
          dict_->putWord(word, record.first);
        }
      }
    }
  }
  multiWordOfOneArticle_.clear();
  if (stub_) {
  } else {
    dict_->writeDB();
  }
}

void OstrichWord::upWordByArticle2Master(const pt::ptree &task) {
  auto wordArrays = pickupWordRanking();
  pt::ptree upTask = task;
  upTask.put("word", wordArrays);
  string task_word_upPath = "/tmp/wai.native/task_word_up.json";
  pt::write_json(task_word_upPath, upTask);
  multiWordOfOneArticle_.clear();
}
