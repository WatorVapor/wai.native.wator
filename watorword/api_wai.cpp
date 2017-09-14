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

static std::shared_ptr<PhoenixWord> gPhoenixChinese;
static std::shared_ptr<PhoenixWord> gPhoenixJapanese;
bool loadMasterDB(void) {
  gPhoenixChinese = std::make_shared<PhoenixWord>("./db/parrot");
  gPhoenixJapanese = std::make_shared<PhoenixWord>("./db/parrot");
  if (gPhoenixChinese->loadMaster(false) == false) {
    return false;
  }
  if (gPhoenixJapanese->loadMaster(false) == false) {
    return false;
  }
  return true;
}
void unloadMasterDB(void) {
  gPhoenixChinese->unloadMaster();
  gPhoenixChinese.reset();
  gPhoenixJapanese->unloadMaster();
  gPhoenixJapanese.reset();
}

#include <boost/exception/all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include <sstream>

string processTextCN(const string &text);
string processTextJA(const string &text);

static bool gJapanese = false;
string processText(const string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      auto lang = langOpt.get();
      if (lang == "jp") {
        gJapanese = true;
      } else {
        gJapanese = false;
      }
    } else {
      gJapanese = false;
    }

  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
    if (gJapanese) {
      return processTextJA(text);
    } else {
      return processTextCN(text);
    }
  }
  return "";
}

string processTextCN(const string &text) {
  pt::ptree resultTotal;
  auto learnPhoenixCh = [&](string wordStr, vector<string> word) {
    DUMP_VAR(gPhoenixChinese);
    double predCN = 0.0;
    auto result = gPhoenixChinese->cut(word, wordStr, predCN);
    DUMP_VAR(predCN);
    /*
        string clearTotalStr;
        pt::ptree resultArray;
        for(auto clearSeq:result) {
          pt::ptree clearPt;
          DUMP_VAR(clearSeq.first);
          clearPt.put(u8"clear",clearSeq.first);
          pt::ptree ambiguityPt;
          DUMP_VAR(clearSeq.second.size());
          int rank =1;
          string clearStr;
          for(auto ambig:clearSeq.second){
            DUMP_VAR(ambig);
            ambiguityPt.put(std::to_string(rank++),ambig);
          }
          clearTotalStr += clearSeq.first;
          clearPt.add_child(u8"ambiguity", ambiguityPt);
          resultArray.push_back(std::make_pair("",clearPt));
        }
        pt::ptree sentencePt;
        sentencePt.put(u8"clear",clearStr);
        sentencePt.add_child(u8"details",resultArray);
        resultCN.push_back(std::make_pair("",sentencePt));
     */
    resultTotal.push_back(std::make_pair("", result));
  };
  CtrlClaw claw;
  claw.claw(text);
  claw.eachSentence(learnPhoenixCh);
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

string processTextJA(const string &text) {
  pt::ptree resultTotal;
  auto learnPhoenixJa = [&](string wordStr, vector<string> word) {
    DUMP_VAR(gPhoenixJapanese.get());
    double predJA = 0.0;
    auto result = gPhoenixJapanese->cut(word, wordStr, predJA);
    DUMP_VAR(predJA);
    /*
        int pos = 1;
        string clearStr;
        pt::ptree resultArray;
        for(auto clearSeq:result) {
          pt::ptree clearPt;
          DUMP_VAR(clearSeq.first);
          clearPt.put(u8"clear",clearSeq.first);
          clearStr += clearSeq.first;
          pt::ptree ambiguityPt;
          DUMP_VAR(clearSeq.second.size());
          int rank =1;
          for(auto ambig:clearSeq.second){
            DUMP_VAR(ambig);
            ambiguityPt.put(std::to_string(rank++),ambig);
          }
          clearPt.add_child(u8"ambiguity", ambiguityPt);
          resultArray.push_back(std::make_pair("",clearPt));
        }
        pt::ptree sentencePt;
        sentencePt.put(u8"clear",clearStr);
        sentencePt.add_child(u8"details",resultArray);
        resultJA.push_back(std::make_pair("",sentencePt));
     */
    resultTotal.push_back(std::make_pair("", result));
  };
  CtrlClaw claw;
  claw.claw(text);
  claw.eachSentence(learnPhoenixJa);
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
