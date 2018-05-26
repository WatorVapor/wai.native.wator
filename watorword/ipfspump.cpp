
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

#include "ipfspump.hpp"


void createIpfsPubSubChannel();

IpfsTextPump::IpfsTextPump() : 
resoureBlock_("") {
  auto uuid = boost::uuids::random_generator()();
  ws_ = "/tmp/wai.native/" + boost::lexical_cast<std::string>(uuid);
  string cmd = "mkdir -p ";
        cmd += ws_;
  ::system(cmd.c_str());
  createIpfsPubSubChannel();
}

#include <thread>

const string strConstRelaySubChannelName("wai.relay.ipfs.to.redis");
const string strConstRelayPubChannelName("wai.relay.redis.to.ipfs");


class RedisRelayClient
{
public:
    RedisRelayClient(boost::asio::io_service &ioService)
        : ioService(ioService)
    {}
    void onMessage(const std::vector<char> &buf);
private:
    boost::asio::io_service &ioService;
};

#include <nlohmann/json.hpp>
using json = nlohmann::json;


void RedisRelayClient::onMessage(const std::vector<char> &buf) {
  string msg(buf.begin(),buf.end());
  DUMP_VAR(msg);
  json j;
  msg >> j;
  DUMP_VAR(j);
}


static std::weak_ptr<redisclient::RedisAsyncClient> gPublishRef;

void ipfs_redis_relay_main(void) {
  boost::asio::io_service ioService;
  boost::asio::ip::tcp::resolver resolver(ioService);
  boost::asio::ip::tcp::resolver::query query("127.0.0.1", "6379");
  boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
  boost::asio::ip::tcp::endpoint endpoint = iter->endpoint();
  DUMP_VAR(endpoint);
  
  auto publish = std::make_shared<redisclient::RedisAsyncClient>(ioService);
  DUMP_VAR(publish);
  publish->connect(endpoint, [&](boost::system::error_code ec) {
    if(ec) {
      DUMP_VAR(ec);
    } else {
      DUMP_VAR(ec);
      gPublishRef = publish;
    }
  });

  redisclient::RedisAsyncClient subscriber(ioService);
  RedisRelayClient client(ioService);
  subscriber.connect(endpoint, [&](boost::system::error_code ec){
    if(ec) {
      DUMP_VAR(ec);
    } else {
      DUMP_VAR(ec);
      subscriber.subscribe(strConstRelaySubChannelName,std::bind(&RedisRelayClient::onMessage, &client, std::placeholders::_1));
    }
  });
  ioService.run();
}

void createIpfsPubSubChannel(void) {
  std::thread t1(ipfs_redis_relay_main);
  t1.detach();
}


IpfsTextPump::~IpfsTextPump() {}



#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;

bool IpfsTextPump::fetchMasterTask(pt::ptree &task, string &content) {
  if(resoureBlock_.empty()) {
    return false;
  }
  
  string taskJSONPath = ws_ + "/task.json";
  string taskTextPath = ws_ + "/task.text";
  string wget = "wget -6 --tries=3 --connect-timeout=10 \"";
  // string wget =  "wget ";
  wget += url_;
  wget += "\" -O ";
  wget += taskJSONPath;
  DUMP_VAR(wget);
  ::system(wget.c_str());
  try {
    pt::ptree taskJson;
    pt::read_json(taskJSONPath, taskJson);
    task = taskJson;
    string taskURL;
    auto taskURLOpt = taskJson.get_optional<string>("url");
    if (taskURLOpt) {
      taskURL = taskURLOpt.get();
      DUMP_VAR(taskURL);
      string textWget = "wget --tries=3 --connect-timeout=10 \"";
      textWget += taskURL;
      textWget += "\" -O ";
      textWget += taskTextPath;
      DUMP_VAR(textWget);
      ::system(textWget.c_str());
      std::ifstream textStream(taskTextPath);
      std::string str((std::istreambuf_iterator<char>(textStream)),
                      std::istreambuf_iterator<char>());
      textStream.close();
      content = str;
      /*
            pt::ptree textXml;
            pt::read_xml(taskTextPath,textXml);
            std::stringstream ss;
            const int indent = 2;
            pt::write_xml(ss,textXml,std::locale(),pt::xml_writer_make_settings('
         ', indent, widen<char>("utf-8")));
            content = ss.str();
      */
    }
  } catch (const pt::json_parser::json_parser_error &e) {
    DUMP_VAR(e.what());
    return false;
  } catch (const std::exception &ex) {
    DUMP_VAR(ex.what());
    return false;
  }
  return true;
}
