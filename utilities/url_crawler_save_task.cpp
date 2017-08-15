#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
using namespace std;


#include <condition_variable>
#include <mutex>

extern string gTask;
extern std::mutex gTaskMutex;
extern std::condition_variable gTaskCV;
extern std::mutex gTaskCvMutex;


static void processTextInSide(const string &text);

void url_crawler_write_main(void){
  while (true) {
    std::unique_lock<std::mutex> lk(gTaskCvMutex);
    gTaskCV.wait(lk);
    string text;
    {
      text = gTask;
    }
    if(text.empty() == false) {
      gNewTaskFlag = false;
      processTextInSide(text);
    }
  }
}


#include <boost/format.hpp>
#include <boost/uuid/sha1.hpp>

#include <cstring>
#include <openssl/sha.h>
#include <regex>
#include <sstream>

static string sha1(const string &data) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((const unsigned char *)(data.c_str()), data.size(), hash);
  std::stringstream ss;
  for (auto dig : hash) {
    ss << hex << setw(2) << setfill('0') << (int)dig;
  }
  return ss.str();
}



#include <atomic>
extern std::atomic_bool gNewTaskFlag; 

#include "urlstorage.hpp"
extern std::shared_ptr<URLStorage> gCNMasterStorage;
extern std::shared_ptr<URLStorage> gCNTodoStorage;
extern std::shared_ptr<URLStorage> gJAMasterStorage;
extern std::shared_ptr<URLStorage> gJATodoStorage;

static void markCrawler(const string &url,const string &lang) {
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  string donePath(WAI_STORAGE + "/" + lang + "/master/" + doneName);
  fs::path pathDoneFS(donePath);
  if (fs::exists(pathDoneFS) == false) {
    ofstream doneMasterFile(donePath);
    if (doneMasterFile.is_open()) {
      doneMasterFile << url;
      doneMasterFile.close();
    }
  }
  string todoPath(WAI_STORAGE + "/" + lang + "/todo/" + doneName);
  fs::path pathFS(todoPath);
  if (fs::exists(pathFS)) {
    DUMP_VAR(fs::exists(pathFS));
    fs::remove(pathFS);
  } else {
    DUMP_VAR2(fs::exists(pathFS),todoPath);
  }
  DUMP_VAR2(fs::exists(pathFS),todoPath);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markCrawler_ms = end - start;
  DUMP_VAR(markCrawler_ms.count());
}

static void newCrawler(const string &url,const string &lang) {
  auto start = std::chrono::system_clock::now();
  auto todoName = sha1(url);
  TRACE_VAR(url, todoName);
  string doneCheckPath(WAI_STORAGE + "/" + lang + "/master/" + todoName);
  fs::path doneCheckPathFS(doneCheckPath);
  DUMP_VAR3(fs::exists(doneCheckPathFS),url,doneCheckPath);
  if (fs::exists(doneCheckPathFS) == false) {
    string todoNewPath(WAI_STORAGE + "/" + lang + "/todo/" + todoName);
    ofstream newURLFile(todoNewPath);
    if (newURLFile.is_open()) {
      newURLFile << url;
      newURLFile.close();
    }
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> newCrawler_ms = end - start;
  DUMP_VAR(newCrawler_ms.count());
}

extern std::shared_ptr<udp_server> gUPDServer;

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
    markCrawler(urlDone,lang);

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
            newCrawler(url,lang);
          }
          if(gNewTaskFlag) {
            DUMP_VAR(gNewTaskFlag);
            break;
          }
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
    gUPDServer->send("failure");
  }
  gUPDServer->send("success");
}



