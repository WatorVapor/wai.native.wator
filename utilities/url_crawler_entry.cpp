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

static const uint16_t iConstAPIPortRangeMin = 41264;
static const uint16_t iConstAPIPortRangeMax = 41274;

#include "log.hpp"
#include "udp_entry.hpp"

static void processText(const std::string &text);



void url_crawler_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstAPIPortRangeMin; port < iConstAPIPortRangeMax;
       port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port,"/watorvapor/wai.storage/conf/url.pool.api.json");
      auto server = std::make_shared<udp_server>(sock);
      server->start_receive(processText);
      DUMP_VAR(server.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}

string fetchCrawlerTask(const string &lang);

void processText(const std::string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      auto lang = langOpt.get();
      DUMP_VAR(lang);
      auto typeOpt = configJson.get_optional<string>("type");
      if (typeOpt) {
        auto type = typeOpt.get();
        DUMP_VAR(type);
        if (type == "crawler") {
          fetchCrawlerTask(lang);
          return;
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
  return ;
}

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
    fs::path path("/watorvapor/wai.storage/cn/todo");
    BOOST_FOREACH (const fs::path &p,
                   std::make_pair(fs::recursive_directory_iterator(path),
                                  fs::recursive_directory_iterator())) {
      if (!fs::is_directory(p)) {
        string pathText = p.string();
        TRACE_VAR(pathText);
        std::ifstream textStream(pathText);
        std::string str((std::istreambuf_iterator<char>(textStream)),
                        std::istreambuf_iterator<char>());
        textStream.close();
        gVectTodoPathCN.push_back(str);
        if (gVectTodoPathCN.size() > iConstPathCacheMax) {
          break;
        }
      }
    }
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
    fs::path path("/watorvapor/wai.storage/ja/todo");
    BOOST_FOREACH (const fs::path &p,
                   std::make_pair(fs::recursive_directory_iterator(path),
                                  fs::recursive_directory_iterator())) {
      if (!fs::is_directory(p)) {
        string pathText = p.string();
        TRACE_VAR(pathText);
        std::ifstream textStream(pathText);
        std::string str((std::istreambuf_iterator<char>(textStream)),
                        std::istreambuf_iterator<char>());
        textStream.close();
        gVectTodoPathJA.push_back(str);
        if (gVectTodoPathJA.size() > iConstPathCacheMax) {
          break;
        }
      }
    }
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
  while (true) {
    findTodoURLs();
    DUMP_VAR(gVectTodoPathCN.size());
    DUMP_VAR(gVectTodoPathJA.size());
    std::unique_lock<std::mutex> lk(gVectoPathCvMutex);
    gVectoPathCV.wait(lk);
  }
}
