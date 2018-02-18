#include "redis_entry.hpp"
#include "log.hpp"

const string strConstTrainChannelName("wai.train");
const string strConstTrainResponseChannelName("wai.train.response");

#include<memory>
#include <chrono>
#include <thread>
#include <atomic>

void redis_sub_main(void) {
  for(;;) {
    try{
      boost::asio::io_service ioService;
      boost::asio::ip::tcp::resolver resolver(ioService);
      boost::asio::ip::tcp::resolver::query query("master.redis.wator.xyz", "6379");
      boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      boost::asio::ip::tcp::endpoint endpoint = iter->endpoint();
      DUMP_VAR(endpoint);
      RedisEntryClient client(ioService);
      redisclient::RedisAsyncClient subscriber(ioService);
      subscriber.connect(endpoint, [&](boost::system::error_code ec){
        if(ec) {
          DUMP_VAR(ec);
        } else {
          DUMP_VAR(ec);
          subscriber.subscribe(strConstTrainChannelName,std::bind(&RedisEntryClient::onMessageAPI, &client, std::placeholders::_1));
        }
      });
      ioService.run();
    } catch(std::exception e) {
      DUMP_VAR(e.what());
    }
    std::this_thread::sleep_for(10s);
  }
}


static std::shared_ptr<redisclient::RedisAsyncClient> gPublish;
static std::atomic_bool gPublishConnected(false);


void redis_pub_main(void) {
  for(;;) {
    try {
      boost::asio::io_service ioService;
      boost::asio::ip::tcp::resolver resolver(ioService);
      boost::asio::ip::tcp::resolver::query query("master.redis.wator.xyz", "6379");
      boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      boost::asio::ip::tcp::endpoint endpoint = iter->endpoint();
      DUMP_VAR(endpoint);
      RedisEntryClient client(ioService);
      gPublish = std::make_shared<redisclient::RedisAsyncClient>(ioService);
      DUMP_VAR(gPublish);
      gPublish->connect(endpoint, [&](boost::system::error_code ec) {
        if(ec) {
          DUMP_VAR(ec);
        } else {
          DUMP_VAR(ec);
          gPublishConnected = true;
        }
      });
      DUMP_VAR(gPublish);
      ioService.run();
    } catch(std::exception e) {
      DUMP_VAR(e.what());
    }
    gPublishConnected = false;
    if(gPublish && gPublish->isConnected()) {
      gPublish->disconnect();
    }
    gPublish= nullptr;
    std::this_thread::sleep_for(10s);
  }
}

#include <nlohmann/json.hpp>
using json = nlohmann::json;

string processText(const string &text);
void RedisEntryClient::onMessageAPI(const std::vector<char> &buf) {
  string msg(buf.begin(),buf.end());
  DUMP_VAR(msg);
  
  auto result = processText(msg);
  TRACE_VAR(result);
  if(result.empty()) {
    auto  emptyObj = R"({"finnish": true})"_json;
    result = emptyObj.dump();
  }
  DUMP_VAR2(gPublish,result);
  if(gPublishConnected && gPublish ) {
    if(gPublish->isConnected()) {
      gPublish->publish(strConstTrainResponseChannelName, result,[&](const redisclient::RedisValue &) {
      });
    } else {
      DUMP_VAR(gPublish->isConnected());
    }
  }
}
