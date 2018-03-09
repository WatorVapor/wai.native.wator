#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include "ctrlclaw.hpp"
#include "log.hpp"
#include "zhiziword.hpp"
#include "textpump.hpp"

static std::shared_ptr<ZhiZiWord> gZhiZi;
bool loadMasterDB(void) {
  gZhiZi = std::make_shared<ZhiZiWord>("./db/zhizi/word","./db/zhizi/phrase");
  if (gZhiZi->loadMaster(false) == false) {
    return false;
  }
  return true;
}
void unloadMasterDB(void) {
  gZhiZi->unloadMaster();
  gZhiZi.reset();
}

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <sstream>

string processWord(const string &text,const string &lang);

static bool gJapanese = false;
static string gLang = "ja";
string processText(const string &text) {
  try {
    json configJson = json::parse(text);
    configJson.find("lang");
    auto langOpt = configJson.find("lang");
    if (langOpt != configJson.end()) {
      gLang = langOpt->get<string>();
    }
    auto sentenceOpt = configJson.find("sentence");
    if (sentenceOpt!= configJson.end()) {
      auto text = sentenceOpt->get<string>();
      return processWord(text,gLang);
      
    }
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
    return "";
  }
  return "";
}

string processWord(const string &text,const string &lang) {
  json resultTotal;
  auto learnZhiZi = [&](string wordStr, vector<string> word,bool multi) {
    //DUMP_VAR(gZhiZi);
    DUMP_VAR(wordStr);
    DUMP_VAR(multi);
    if(multi) {
      auto result = gZhiZi->cut(word, wordStr, lang);
      resultTotal.push_back(result);
    } else {
      auto result = gZhiZi->cutSpace(word, wordStr, lang);
      resultTotal.push_back(result);
    }
  };
  CtrlClaw claw;
  claw.claw(text);
  claw.eachSentenceMix(learnZhiZi);
  try {
    json result;
    result[u8"wai"] = resultTotal;
    return result.dump();;
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  }
  return "";
}

