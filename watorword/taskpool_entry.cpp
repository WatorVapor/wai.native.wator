#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
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
static const uint16_t iConstAPIPortRangeMax = 41284;
static const uint32_t iConstMSGBufferMax = 1024 * 1024;

#include "log.hpp"

string processText(const string &text);

class udp_server {
public:
  udp_server(shared_ptr<udp::socket> sock)
      : socket_(sock) {
    start_receive();
  }
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
    DUMP_VAR(remote_endpoint_);
    DUMP_VAR(bytes_transferred);
    std::string recv_str(recv_buffer_.data(), bytes_transferred);
    DUMP_VAR(recv_str);
    auto reuslt = processText(recv_str);
    if(reuslt.empty() == false) {
      this->send(reuslt);
    }
    if (!error || error == boost::asio::error::message_size) {
      start_receive();
    }
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
    pt::write_json("/watorvapor/wai.storage/conf/task.api.json", portConf);
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

void taskpool_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstAPIPortRangeMin; port < iConstAPIPortRangeMax;
       port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port);
      auto server = std::make_shared<udp_server>(sock);
      DUMP_VAR(server.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}


string fetchCrawlerTask(const string &lang);

string processText(const string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt  = configJson.get_optional<string>("lang");
    if(langOpt) {
      auto lang = langOpt.get();
      DUMP_VAR(lang);
      auto typeOpt  = configJson.get_optional<string>("type");
      if(typeOpt) {
        auto type = typeOpt.get();
        DUMP_VAR(type);
        if(type == "crawler") {
          return fetchCrawlerTask(lang);
        }
      }
    } 
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
  return "";
}

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <mutex>
#include <condition_variable>


static vector<string> gVectTodoPathCN;
static vector<string> gVectTodoPathJA;
static std::mutex gVectoPathMutex;
static std::condition_variable gVectoPathCV;
static std::mutex gVectoPathCvMutex;

static int iConstPathCacheMax 1024;

string fetchCrawlerTask(const string &lang) {
  DUMP_VAR(lang);
  if(lang == "cn") {
    std::lock_guard<std::mutex> lock(gVectoPathMutex);
    if(gVectTodoPathCN.empty()){
      gVectoPathCV.notify_all();
      return "";
    } else {
      auto top = gVectTodoPathCN.back();
      DUMP_VAR(top);
      gVectTodoPathCN.pop_back();
      return top;
    }
  } else if(lang == "ja") {
    std::lock_guard<std::mutex> lock(gVectoPathMutex);
    if(gVectTodoPathJA.empty()){
      gVectoPathCV.notify_all();
      return "";
    } else {
      auto top = gVectTodoPathJA.back();
      DUMP_VAR(top);
      gVectTodoPathJA.pop_back();
      return top;
    }
  } else {
  }
  return "";
}


static void findTodoURLs(void) {
  {
    fs::path path("/watorvapor/wai.storage/cn/todo");
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        string pathText= p.string();
        TRACE_VAR(pathText); 
        std::ifstream textStream(pathText);
        std::string str((std::istreambuf_iterator<char>(textStream)),
                       std::istreambuf_iterator<char>());
        textStream.close();
        gVectTodoPathCN.push_back(str);
        if(gVectTodoPathCN.size() >iConstPathCacheMax){
          break;
        }
      }
    }
  }
  {
    fs::path path("/watorvapor/wai.storage/ja/todo");
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        string pathText= p.string();
        TRACE_VAR(pathText); 
        std::ifstream textStream(pathText);
        std::string str((std::istreambuf_iterator<char>(textStream)),
                       std::istreambuf_iterator<char>());
        textStream.close();
        gVectTodoPathJA.push_back(str);
        if(gVectTodoPathJA.size() >iConstPathCacheMax){
          break;
        }
      }
    }
  }
}


void taskpool_collect(void) {
  while(true) {
    findTodoURLs();
    DUMP_VAR(gVectTodoPathCN.size());
    DUMP_VAR(gVectTodoPathJA.size());
    std::unique_lock<std::mutex> lk(gVectoPathCvMutex);
    gVectoPathCV.wait(lk);
  }
}
