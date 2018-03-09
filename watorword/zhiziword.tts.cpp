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



vector<string> ZhiZiWord::createAudioList(void) {
  vector<string> result;
  for(auto wordSed:wordSeqTopSelected_) {
    auto word = std::get<0>(wordSed.second);
    DUMP_VAR(word);
  }
  return result;
}
