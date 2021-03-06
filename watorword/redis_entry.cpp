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
      /*
      boost::asio::ip::tcp::resolver resolver(ioService);
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", "6379");
      boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      */
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6379);
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
    } catch(std::exception &e) {
      DUMP_VAR(e.what());
    }
    std::this_thread::sleep_for(10s);
  }
}


static std::weak_ptr<redisclient::RedisAsyncClient> gPublishRef;
//static std::atomic_bool gPublishConnected(false);


void redis_pub_main(void) {
  for(;;) {
    try {
      boost::asio::io_service ioService;
      /*
      boost::asio::ip::tcp::resolver resolver(ioService);
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", "6379");
      boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      */
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6379);;
      DUMP_VAR(endpoint);
      RedisEntryClient client(ioService);
      auto publish = std::make_shared<redisclient::RedisAsyncClient>(ioService);
      DUMP_VAR(publish);
      gPublishRef = publish;
      publish->connect(endpoint, [&](boost::system::error_code ec) {
        if(ec) {
          DUMP_VAR(ec);
        } else {
          DUMP_VAR(ec);
          //gPublishConnected = true;
        }
      });
      DUMP_VAR(publish);
      ioService.run();
    } catch(std::exception &e) {
      DUMP_VAR(e.what());
    }
    //gPublishConnected = false;
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
  auto publish = gPublishRef.lock();
  DUMP_VAR2(publish,result);
  if(publish &&publish->isConnected()) {
      publish->publish(strConstTrainResponseChannelName, result,[&](const redisclient::RedisValue &) {
      });
  } else {
      DUMP_VAR(publish);
  }
}
