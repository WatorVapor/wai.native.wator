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


string createTTSLink(json clipList);

string processWord(const string &text,const string &lang) {
  json resultTotal;
  json ttsTotal;
  auto learnZhiZi = [&](string wordStr, vector<string> word,bool multi) {
    //DUMP_VAR(gZhiZi);
    DUMP_VAR(wordStr);
    DUMP_VAR(multi);
    if(multi) {
      auto result = gZhiZi->cut(word, wordStr, lang);
      for (auto& element : result["tts"]) {
        ttsTotal.push_back(element);
      }
      result.erase("tts");
      resultTotal.push_back(result);
    } else {
      auto result = gZhiZi->cutSpace(word, wordStr, lang);
      for (auto& element : result["tts"]) {
        ttsTotal.push_back(element);
      }
      result.erase("tts");
      resultTotal.push_back(result);
    }
  };
  CtrlClaw claw;
  claw.claw(text);
  claw.eachSentenceMix(learnZhiZi);
  try {
    json result;
    result[u8"wai"] = resultTotal;
    
    
    result[u8"tts"] = createTTSLink(ttsTotal);

    return result.dump();;
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  }
  return "";
}

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

string createTTSLink(json clipList) {
  for (auto& clip : clipList) {
    DUMP_VAR(clip);
  }
  auto id = boost::uuids::random_generator()();
  auto fileName = boost::lexical_cast<std::string>(id);
  ::system("mkdir -p /watorvapor/autogen/wator/wai/audio/");
  string url = "/autogen/wai/audio/";
  url += id;
  url += ".m3u8";
  return url;
}

