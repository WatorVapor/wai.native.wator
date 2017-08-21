#include <chrono>
#include <cinttypes>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>
using namespace std;

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "log.hpp"

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include <boost/algorithm/string.hpp>

extern std::atomic_bool gNewTaskFlag;
extern string gTask;
extern std::mutex gTaskMutex;
extern std::condition_variable gTaskCV;
extern std::mutex gTaskCvMutex;

static void processTextInSide(const string &text);

void url_crawler_write_main(void) {
  while (true) {
    std::unique_lock<std::mutex> lk(gTaskCvMutex);
    gTaskCV.wait(lk);
    string text;
    { text = gTask; }
    if (text.empty() == false) {
      gNewTaskFlag = false;
      processTextInSide(text);
    }
  }
}

#include <boost/format.hpp>
#include <boost/uuid/sha1.hpp>

#include <openssl/sha.h>
#include <cstring>
#include <regex>
#include <sstream>

string sha1(const string &data) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((const unsigned char *)(data.c_str()), data.size(), hash);
  std::stringstream ss;
  for (auto dig : hash) {
    ss << hex << setw(2) << setfill('0') << (int)dig;
  }
  return ss.str();
}

#include "urlstorage.hpp"
extern std::shared_ptr<URLStorage> gCNMasterStorage;
extern std::shared_ptr<URLStorage> gCNTodoStorage;
extern std::shared_ptr<URLStorage> gJAMasterStorage;
extern std::shared_ptr<URLStorage> gJATodoStorage;

static void markCrawler(const string &url, const string &lang) {
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  if (lang == "cn") {
    gCNMasterStorage->add(doneName, url);
    gCNTodoStorage->remove(doneName);
  } else if (lang == "ja") {
    gJAMasterStorage->add(doneName, url);
    gJATodoStorage->remove(doneName);
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markCrawler_ms = end - start;
  DUMP_VAR(markCrawler_ms.count());
}

static void newCrawler(const string &url, const string &lang) {
  auto start = std::chrono::system_clock::now();
  auto todoName = sha1(url);
  TRACE_VAR(url, todoName);

  if (lang == "cn") {
    if (gCNMasterStorage->is_has(todoName) == false) {
      gCNTodoStorage->add(todoName, url);
    }
  } else if (lang == "ja") {
    if (gJAMasterStorage->is_has(todoName) == false) {
      gJATodoStorage->add(todoName, url);
    }
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> newCrawler_ms = end - start;
  DUMP_VAR(newCrawler_ms.count());
}

#include "udp_entry.hpp"
extern std::shared_ptr<udp_server> gSaveServer;

void processTextInSide(const string &text) {
  try {
    TRACE_VAR(text);
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    std::string lang;
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      lang = langOpt.get();
    }
    std::string urlDone;
    auto urlOpt = configJson.get_optional<string>("url");
    if (urlOpt) {
      urlDone = urlOpt.get();
    }
    markCrawler(urlDone, lang);

    auto it = configJson.find("crawler");
    if (it != configJson.not_found()) {
      auto crawlerOpt = configJson.get_optional<string>("crawler");
      if (crawlerOpt) {
        auto crawler = crawlerOpt.get();
        TRACE_VAR(crawler);
        list<string> list_string;
        string delim("{};");
        boost::split(list_string, crawler, boost::is_any_of(delim),
                     boost::algorithm::token_compress_on);
        DUMP_VAR(list_string.size());
        for (auto url : list_string) {
          if (url.empty() == false) {
            newCrawler(url, lang);
          }
          if (gNewTaskFlag) {
            DUMP_VAR(gNewTaskFlag);
            break;
          }
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
    gSaveServer->send("failure");
  }
  gSaveServer->send("success");
}
