
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include <redisclient/redisasyncclient.h>

#include "ipfspump.hpp"


void createIpfsPubSubChannel();

IpfsTextPump::IpfsTextPump()
{
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

static string gBlockCID = "";
static string gTask = "";
static mutex gBlockMutex;

void RedisRelayClient::onMessage(const std::vector<char> &buf) {
  //string msg(buf.begin(),buf.end());
  //DUMP_VAR(msg);
  json jsonMsg = json::parse(buf.begin(),buf.end());
  DUMP_VAR(jsonMsg);
  auto jsonTask = jsonMsg["task"];
  DUMP_VAR(jsonTask);
  if(jsonTask.is_string()){
    auto task = jsonTask.get<std::string>();
    if(task == "wator.ipfs.ostrich.app") {
      DUMP_VAR(task);
      {
        std::lock_guard<std::mutex> guard(gBlockMutex);
        gTask = task;
      }
      auto jsonBlock = jsonMsg["block"];
      if(jsonBlock.is_string()){
        auto block = jsonBlock.get<std::string>();
        DUMP_VAR(block);
        if(boost::starts_with(block,"Qm")) {
          std::lock_guard<std::mutex> guard(gBlockMutex);
          gBlockCID = block;
          DUMP_VAR(gBlockCID);
        }
      }
    }
  }
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
void commitIpfs(const json &response){
  DUMP_VAR(response);
  std::string serialized_string = response.dump();
  if(gPublishRef) {
    gPublishRef->publish(strConstRelayPubChannelName,response);
  }
};


IpfsTextPump::~IpfsTextPump() {}



#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;

string IpfsTextPump::fetchIpfsResource(const string &cid) {
  string result;
  string urlIpfs = url_ + "/" + cid;
  DUMP_VAR(urlIpfs);
  
  string taskTextPath = ws_ + "/ipfs.text";
  string wget = "wget -6 --tries=3 --connect-timeout=10 \"";
  // string wget =  "wget ";
  wget += urlIpfs;
  wget += "\" -O ";
  wget += taskTextPath;
  DUMP_VAR(wget);
  ::system(wget.c_str());
  try {  
    std::ifstream textStream(taskTextPath);
    std::string str((std::istreambuf_iterator<char>(textStream)),
                    std::istreambuf_iterator<char>());
    result = str;
    textStream.close();
  } catch (const std::exception &ex) {
    DUMP_VAR(ex.what());
  }
  return result;
}

void IpfsTextPump::parseResourceBlock(const string & block) {
  json jsonBlock = json::parse(block.begin(),block.end());
  DUMP_VAR(jsonBlock);
  auto resourceJson = jsonBlock["resource"];
  
  for (auto& element : resourceJson) {
    auto resoureCID = element.get<std::string>();
    DUMP_VAR(resoureCID);
    if(boost::starts_with(resoureCID,"Qm")) {
      resoureCIDs_.push_back(resoureCID);
    }
  }
  auto groupJson = jsonBlock["group"];
  if(groupJson.is_string()) {
    this->group_ = groupJson.get<std::string>();
  }
}

bool IpfsTextPump::fetchBlockResource(void) {
  std::lock_guard<std::mutex> guard(gBlockMutex);
  if(gBlockCID.empty()) {
    return false;
  }
  DUMP_VAR(gBlockCID);
  auto blocks = this->fetchIpfsResource(gBlockCID);
  DUMP_VAR(blocks);
  this->parseResourceBlock(blocks);
  this->task_ = gTask;
  gBlockCID = "";
  gTask = "";
  return true;
}


bool IpfsTextPump::fetchMasterTask(json &task, string &content) {
  if(resoureCIDs_.empty()) {
    this->fetchBlockResource();
    return false;
  }
  auto cid = resoureCIDs_.back();
  content = fetchIpfsResource(cid);
  task["task"] = this->task_;
  task["group"] = this->group_;
  DUMP_VAR(this->task_);
  DUMP_VAR(this->group_);
  TRACE_VAR(content);
  resoureCIDs_.pop_back();
  return true;
}



