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
#include "phoenixword.hpp"
#include "textpump.hpp"

static std::shared_ptr<PhoenixWord> gPhoenix;
bool loadMasterDB(void) {
  gPhoenix = std::make_shared<PhoenixWord>("./db/parrot");
  if (gPhoenix->loadMaster(false) == false) {
    return false;
  }
  return true;
}
void unloadMasterDB(void) {
  gPhoenix->unloadMaster();
  gPhoenix.reset();
}

#include <boost/exception/all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include <sstream>

string processWord(const string &text,const string &lang);

static bool gJapanese = false;
static string gLang = "ja";
string processText(const string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      gLang = langOpt.get();
      return "";
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
    return processWord(text,gLang);
  }
  return "";
}

string processWord(const string &text,const string &lang) {
  pt::ptree resultTotal;
  auto learnPhoenix = [&](string wordStr, vector<string> word) {
    DUMP_VAR(gPhoenix);
    auto result = gPhoenix->cut(word, wordStr, lang);
    resultTotal.push_back(std::make_pair("", result));
  };
  CtrlClaw claw;
  claw.claw(text);
  claw.eachSentence(learnPhoenix);
  try {
    pt::ptree result;
    result.add_child(u8"wai", resultTotal);
    std::stringstream ss;
    pt::write_json(ss, result);
    return ss.str();
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
  return "";
}

