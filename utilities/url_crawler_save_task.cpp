#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/all.hpp>
using namespace boost::asio::ip;

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <condition_variable>
#include <mutex>

static vector<string> gVectTodoPathCN;
static vector<string> gVectTodoPathJA;
static std::mutex gVectoPathMutex;
static std::condition_variable gVectoPathCV;
static std::mutex gVectoPathCvMutex;

static int iConstPathCacheMax = 32;

string fetchCrawlerTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
    std::lock_guard<std::mutex> lock(gVectoPathMutex);
    if (gVectTodoPathCN.empty()) {
      gVectoPathCV.notify_all();
      return "";
    } else {
      auto top = gVectTodoPathCN.back();
      DUMP_VAR(top);
      gVectTodoPathCN.pop_back();
      DUMP_VAR(gVectTodoPathCN.size());
      return top;
    }
  } else if (lang == "ja") {
    std::lock_guard<std::mutex> lock(gVectoPathMutex);
    if (gVectTodoPathJA.empty()) {
      gVectoPathCV.notify_all();
      return "";
    } else {
      auto top = gVectTodoPathJA.back();
      DUMP_VAR(top);
      gVectTodoPathJA.pop_back();
      DUMP_VAR(gVectTodoPathJA.size());
      return top;
    }
  } else {
  }
  return "";
}

#include "urlstorage.hpp"
std::shared_ptr<URLStorage> gCNMasterStorage;
std::shared_ptr<URLStorage> gCNTodoStorage;
std::shared_ptr<URLStorage> gJAMasterStorage;
std::shared_ptr<URLStorage> gJATodoStorage;

static void findToduURLsCN(void) {
  try {
    gCNTodoStorage->gets(iConstPathCacheMax,gVectTodoPathCN);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  // add seed
  if (gVectTodoPathCN.empty()) {
    gVectTodoPathCN.push_back(
        "https://zh.wikipedia.org/zh-cn/%E7%94%B5%E5%AD%90");
  }
}

static void findToduURLsJA(void) {
  try {
    gJATodoStorage->gets(iConstPathCacheMax,gVectTodoPathJA);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  if (gVectTodoPathJA.empty()) {
    gVectTodoPathJA.push_back(
        "https://ja.wikipedia.org/wiki/%E9%9B%BB%E5%AD%90");
  }
}


static void findTodoURLs(void) {
  if (gVectTodoPathCN.empty()) {
    findToduURLsCN();
  }
  if (gVectTodoPathJA.empty()) {
    findToduURLsJA();
  }
}

void url_crawler_collect(void) {
  gCNMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/master");
  gCNTodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/todo");
  gJAMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/master");
  gJATodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/todo");
  gCNMasterStorage->openDB();
  gCNTodoStorage->openDB();
  gJAMasterStorage->openDB();
  gJATodoStorage->openDB();
  while (true) {
    findTodoURLs();
    DUMP_VAR(gVectTodoPathCN.size());
    DUMP_VAR(gVectTodoPathJA.size());
    std::unique_lock<std::mutex> lk(gVectoPathCvMutex);
    gVectoPathCV.wait(lk);
  }
  gCNMasterStorage->writeDB();
  gCNTodoStorage->writeDB();
  gJAMasterStorage->writeDB();
  gJATodoStorage->writeDB();
  gCNMasterStorage->closeDB();
  gCNTodoStorage->closeDB();
  gJAMasterStorage->closeDB();
  gJATodoStorage->closeDB();
}

