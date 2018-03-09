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


static const TTSMaster gCN("./db/tts/cn");
static const TTSMaster gJA("./db/tts/ja");

vector<string> ZhiZiWord::createAudioList(const string &lang) {
  vector<string> result;
  DUMP_VAR(lang);
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
  }
  return result;
}
