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
#include "qiangbaoword.hpp"
#include "textpump.hpp"

static std::shared_ptr<QiangbaoWord> gQiangbaoChinese;
static std::shared_ptr<QiangbaoWord> gQiangbaoJapanese;
bool loadMasterDB(void) {
  gQiangbaoChinese = std::make_shared<QiangbaoWord>("./db/baidu.baike");
  gQiangbaoJapanese = std::make_shared<QiangbaoWord>("./db/wiki.ja");
  if (gQiangbaoChinese->loadMaster(false) == false) {
    return false;
  }
  if (gQiangbaoJapanese->loadMaster(false) == false) {
    return false;
  }
  return true;
}
void unloadMasterDB(void) {
  gQiangbaoChinese->unloadMaster();
  gQiangbaoChinese.reset();
  gQiangbaoJapanese->unloadMaster();
  gQiangbaoJapanese.reset();
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
  auto learnQiangbaoCh = [&](string wordStr, vector<string> word) {
    DUMP_VAR(gQiangbaoChinese);
    double predCN = 0.0;
    auto result = gQiangbaoChinese->cut(word, wordStr, predCN);
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
  claw.eachSentence(learnQiangbaoCh);
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
  auto learnQiangbaoJa = [&](string wordStr, vector<string> word) {
    DUMP_VAR(gQiangbaoJapanese.get());
    double predJA = 0.0;
    auto result = gQiangbaoJapanese->cut(word, wordStr, predJA);
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
  claw.eachSentence(learnQiangbaoJa);
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
