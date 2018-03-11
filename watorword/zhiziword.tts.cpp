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
  vector<string> phonemeList;
  DUMP_VAR(lang);
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
    auto phoneme = createPhoneme(word,lang);
    phonemeList.insert(phonemeList.end(),phoneme.begin(),phoneme.end());
  }
  
  vector<string> result;
  if(lang == "cn") {
    for(auto phoneme:phonemeList) {
      auto clipAudio = gCN.getPhoneme(phoneme);
      if(clipAudio.empty() == false) {
        result.push_back(clipAudio);
      } else {
        DUMP_VAR2(phoneme,clipAudio);
      }
    }
  }
  
  if(lang == "ja") {
    for(auto phoneme:phonemeList) {
      auto clipAudio = gJA.getPhoneme(phoneme);
      if(clipAudio.empty() == false) {
        result.push_back(clipAudio);
      } else {
        DUMP_VAR2(phoneme,clipAudio);
      }
    }
  }
  return result;
}

#include <boost/algorithm/string.hpp>

vector<string> ZhiZiWord::createPhoneme(const string &word,const string &lang) {
  vector<string> result;
  if(lang == "cn") {
    result = createPhonemeCN(word);
  }
  if(lang == "ja") {
    result = createPhonemeJA(word);
  }
  result.push_back(u8" ");
  return result;
}


vector<string> ZhiZiWord::createPhonemeCN(const string &word) {
  vector<string> result;
  auto phrasePinyin = gCN.getPhrase(word);
  if(phrasePinyin.empty()) {
    auto hanziA = parseUTF8(word);
    for(auto hanzi :hanziA) {
      auto pinyin = gCN.getHanzi(hanzi);
      if(pinyin.empty()) {
        result.push_back("wm");
      } else {
        std::vector<std::string> results;
        boost::algorithm::split(results, pinyin, boost::algorithm::is_any_of(","));
        if(results.size() > 0) {
          result.push_back(results.at(0));
        }
      }
    }
  } else {
    std::vector<std::string> results;
    boost::algorithm::split(results, phrasePinyin, boost::algorithm::is_any_of(","));
    result.insert(result.end(),results.begin(),results.end());
  }
  result.push_back(u8" ");
  result.push_back(u8"ws");
 return result;
}


vector<string> jaHirakaraPair(const vector<string> &hirakana);

vector<string> ZhiZiWord::createPhonemeJA(const string &word) {
  vector<string> result;
  auto phraseKana = gJA.getPhrase(word);
  if(phraseKana.empty()) {
    auto jHanKaHiA = parseUTF8(word);
    for(auto jHanKaHi :jHanKaHiA) {
      auto kana = gJA.getHanzi(jHanKaHi);
      if(kana.empty()) {
        result.push_back("wm");
      } else {
        std::vector<std::string> results;
        boost::algorithm::split(results, kana, boost::algorithm::is_any_of(","));
        if(results.size() > 0) {
          result.push_back(results.at(0));
        }
      }
    }
  } else {
    std::vector<std::string> results;
    boost::algorithm::split(results, phraseKana, boost::algorithm::is_any_of(","));
    result.insert(result.end(),results.begin(),results.end());
  }
  auto result2 = jaHirakaraPair(result);
  result2.push_back(u8"ws");
  return result2;
}

vector<string> jaHirakaraPair(const vector<string> &hirakana) {
  vector<string> result;
  auto it =  hirakana.begin();
  while(it != hirakana.end()) {
    auto clipAudio1 = gJA.getPhoneme(*it);
    string clipAudio2;
    auto next = std::next(it,1);
    if(next != hirakana.end()) {
      clipAudio2 = gJA.getPhoneme(*it + *next);
    }
    auto next2 = std::next(it,2);
    string clipAudio3;
    if(next != hirakana.end() && next2 != hirakana.end()) {
      clipAudio3 = gJA.getPhoneme(*it + *next + *next2);
    }
    if(clipAudio3.empty()) {
      if(clipAudio2.empty()) {
        result.push_back(*it);
        it = next;
      } else {
        result.push_back(*it + *next);
        it = next2;
      }
    } else {
      result.push_back(*it + *next + *next2);
      it = std::next(it,3);
    }
  }
  return result;
}

