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

void OstrichWord::commitArticle(const pt::ptree &task,const string &ws) {
  auto wordArrays = pickupWordRanking();
  multiWordOfOneArticle_.clear();
  for (auto word : wordArrays) {
    DUMP_VAR(word);
    pt::ptree upTask = task;
    upTask.put("word", word);
    string task_word_upPath = ws + "/task_word_up.json";
    pt::write_json(task_word_upPath, upTask);

    auto tagOpt = task.get_optional<string>("tag");
    if (tagOpt) {
      auto tag = tagOpt.get();
      string wgetTaskUp("curl -6 -F \"");
      // string wgetTaskUp("curl -F \"");
      wgetTaskUp += "file=@";
      wgetTaskUp += task_word_upPath;
      wgetTaskUp += "\" ";
      wgetTaskUp += "\"https://www.wator.xyz/wai/text/train/ostrich/";
      wgetTaskUp += tag;
      wgetTaskUp += "\"";
      DUMP_VAR(wgetTaskUp);
      ::system(wgetTaskUp.c_str());
    }
  }
}
