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

static const uint16_t iConstAPIPortRangeMin = 41284;
static const uint16_t iConstAPIPortRangeMax = 41294;
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
    TRACE_VAR(remote_endpoint_);
    TRACE_VAR(bytes_transferred);
    std::string recv_str(recv_buffer_.data(), bytes_transferred);
    TRACE_VAR(recv_str);
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
    pt::write_json("/watorvapor/wai.storage/conf/task.merge.api.json", portConf);
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

void taskmerge_upd_main(void) {
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

#include <boost/uuid/sha1.hpp>
#include <boost/format.hpp>

#include <openssl/sha.h>
#include <cstring>
#include <sstream>
#include <regex>

static string sha1(const string &data) {
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char*)(data.c_str()), data.size(), hash);
	std::stringstream ss; 
	for(auto dig:hash) {
		ss << hex << setw(2) << setfill('0') << (int)dig;
	}
  return ss.str();
/*	
  boost::uuids::detail::sha1 s;
  s.process_bytes(data.c_str(), data.size());
  unsigned int digest[5];
  s.get_digest(digest);
  string hash;
  for(int i = 0; i < 5; ++i) {
		const char* tmp = reinterpret_cast<char*>(digest);
    hash += (boost::format("%02x")%tmp[i*4+3]).str();
    hash += (boost::format("%02x")%tmp[i*4+2]).str();
    hash += (boost::format("%02x")%tmp[i*4+1]).str();
    hash += (boost::format("%02x")%tmp[i*4+0]).str();

    const std::string s = (boost::format("%2% %1%") % 3 % std::string("Hello")).str();
    hash.append(tmp[i*4+3]));
    hash.append(tmp[i*4+2]);
    hash.append(tmp[i*4+1]);
    hash.append(tmp[i*4]);
  }
 */
}

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

const string WAI_STORAGE = "/watorvapor/wai.storage";
const string WAI_STORAGE_CN = "/watorvapor/wai.storage/cn/todo/";
const string WAI_STORAGE_JA = "/watorvapor/wai.storage/ja/todo/";

string processText(const string &text) {
  try {
    TRACE_VAR(text);
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    std::string lang;
    auto langOpt  = configJson.get_optional<string>("lang");
    if(langOpt) {
      lang = langOpt.get();
    } 
    std::string urlDone;
    auto urlOpt  = configJson.get_optional<string>("url");
    if(urlOpt) {
      urlDone = urlOpt.get();
    }
    auto doneName = sha1(urlDone);
    DUMP_VAR2(doneName,urlDone);
    string donePath = WAI_STORAGE;
    donePath += "/";
    donePath += lang;
    donePath += "/master/";
    donePath +=  doneName;
    
    ofstream doneMasterFile(donePath);
    if(doneMasterFile.is_open()) {
      doneMasterFile << urlDone;
      doneMasterFile.close();
    }
    
    
    string todoPath = WAI_STORAGE;
    todoPath += "/";
    todoPath += lang;
    todoPath += "/todo/";
    todoPath +=  doneName;
    fs::path pathFS(todoPath);
    if(fs::exists(pathFS)) {
      fs::remove(pathFS);
    }
    
    auto it =configJson.find("crawler");
    if(it != configJson.not_found()) {
      auto crawlerOpt = configJson.get_optional<string>("crawler");
      if(crawlerOpt) {
        auto crawler = crawlerOpt.get();
        DUMP_VAR(crawler);
        list<string> list_string;
        string delim ("{};");
        boost::split(list_string, crawler, boost::is_any_of(delim),boost::algorithm::token_compress_on);
        for(auto url:list_string){
          if(url.empty() == false)
            auto todoName = sha1(url);
            DUMP_VAR2(url,todoName);
            string doneCheckPath = WAI_STORAGE;
            doneCheckPath += "/";
            doneCheckPath += lang;
            doneCheckPath += "/master/";
            doneCheckPath +=  todoName;
            fs::path doneCheckPathFS(doneCheckPath);
            if(fs::exists(doneCheckPathFS) == false) {
              string todoNewPath = WAI_STORAGE;
              todoNewPath += "/";
              todoNewPath += lang;
              todoNewPath += "/todo/";
              todoNewPath +=  doneName;
              ofstream doneTodoFile(todoNewPath);
              if(doneTodoFile.is_open()) {
                doneTodoFile << url;
                doneTodoFile.close();
              }
            }
          }
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
  return "";
}

