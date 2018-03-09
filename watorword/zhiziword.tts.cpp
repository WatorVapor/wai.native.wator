#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;


#include "log.hpp"
#include "zhiziword.hpp"
#include "ttsmaster.hpp"


static const TTSMaster gCN("./db/tts/cn");
static const TTSMaster gJA("./db/tts/ja");

vector<string> parseUTF8(const string &words);

vector<string> ZhiZiWord::createAudioList(const string &lang) {
  vector<string> result;
  DUMP_VAR(lang);
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
    auto phoneme = createPhoneme(word,lang);
    result.insert(result.end(),phoneme.begin(),phoneme.end());
  }
  return result;
}

#include <boost/algorithm/string.hpp>

vector<string> ZhiZiWord::createPhoneme(const string &word,const string &lang) {
  vector<string> result;
  if(lang == "cn") {
    auto phrasePinyin = gCN.getPhrase(word);
    if(phrasePinyin.empty()) {
      auto hanziA = parseUTF8(word);
      for(auto hanzi :hanziA) {
        auto pinyin = gCN.getHanzi(hanzi);
        if(pinyin.empty() == false) {
          result.push_back(pinyin);
        } else {
          result.push_back("miss hanzi");
        }
      }
    } else {
      std::vector<std::string> results;
      boost::algorithm::split(results, phrasePinyin, boost::algorithm::is_any_of(","));
      result.insert(result.end(),results.begin(),results.end());
    }
  }
  result.push_back("word space")
}
