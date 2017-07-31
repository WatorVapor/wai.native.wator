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

static const uint16_t iConstAPIPortRangeMin = 41284;
static const uint16_t iConstAPIPortRangeMax = 41294;
static const uint32_t iConstMSGBufferMax = 20*1024 * 1024;

#include "log.hpp"

void processText(const string &text);

class udp_server {
public:
  udp_server(shared_ptr<udp::socket> sock) : socket_(sock) { start_receive(); }
  void send(const std::string &msg) {
    boost::shared_ptr<std::string> message(new std::string(msg));
    socket_->async_send_to(
        boost::asio::buffer(*message), remote_endpoint_,
        boost::bind(&udp_server::handle_send, this, message));
  }

private:
  void start_receive() {
    socket_->async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code &error,
                      std::size_t bytes_transferred) {
    auto start = std::chrono::system_clock::now();
    TRACE_VAR(remote_endpoint_);
    TRACE_VAR(bytes_transferred);
    std::string recv_str(recv_buffer_.data(), bytes_transferred);
    TRACE_VAR(recv_str);
    processText(recv_str);
    if (!error || error == boost::asio::error::message_size) {
      start_receive();
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - start;
    DUMP_VAR(fp_ms.count());
  }

  void handle_send(boost::shared_ptr<std::string> msg) {
    DUMP_VAR(msg);
    DUMP_VAR(*msg);
  }

  shared_ptr<udp::socket> socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, iConstMSGBufferMax> recv_buffer_;
};

static void savePort(uint16_t port) {
  try {
    pt::ptree portConf;
    portConf.put("port", port);
    pt::write_json("/watorvapor/wai.storage/conf/task.merge.api.json",
                   portConf);
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

static std::shared_ptr<udp_server> gUPDServer;
void wordmaster_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstAPIPortRangeMin; port < iConstAPIPortRangeMax;
       port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port);
      gUPDServer = std::make_shared<udp_server>(sock);
      DUMP_VAR(gUPDServer.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}

string fetchCrawlerTask(const string &lang);

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

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
namespace fs = boost::filesystem;

const string WAI_STORAGE = "/watorvapor/wai.storage";
const string WAI_STORAGE_CN = "/watorvapor/wai.storage/cn/todo/";
const string WAI_STORAGE_JA = "/watorvapor/wai.storage/ja/todo/";

#include <atomic>
static std::atomic_bool gNewTaskFlag(false); 

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

#include <condition_variable>
#include <mutex>

static string gTask;
static std::mutex gTaskMutex;
static std::condition_variable gTaskCV;
static std::mutex gTaskCvMutex;

void processText(const string &text) {
  std::lock_guard<std::mutex> lock(gTaskMutex);
  gTask = text;
  gNewTaskFlag = true;
  gTaskCV.notify_all();
}

void wordmaster_write_main(void){
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
