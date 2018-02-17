#include "redis_entry.hpp"
#include "log.hpp"

const string strConstTrainChannelName("wai.train");
const string strConstTrainResponseChannelName("wai.train.response");

#include<memory>

void redis_sub_main(void) {
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
}


std::shared_ptr<redisclient::RedisAsyncClient> gPublish;

void redis_pub_main(void) {
  try{
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::resolver::query query("master.redis.wator.xyz", "6379");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = iter->endpoint();
    DUMP_VAR(endpoint);
    RedisEntryClient client(ioService);
    gPublish = std::make_shared<redisclient::RedisAsyncClient>(ioService);
    gPublish->connect(endpoint, [&](boost::system::error_code ec) {
      if(ec) {
        DUMP_VAR(ec);
      } else {
        DUMP_VAR(ec);
      }
    });
    ioService.run();
  } catch(std::exception e) {
    DUMP_VAR(e.what());
  }
}


string processText(const string &text);
void RedisEntryClient::onMessageAPI(const std::vector<char> &buf) {
  string msg(buf.begin(),buf.end());
  DUMP_VAR(msg);
  
  auto result = processText(msg);
  DUMP_VAR(result);
  if(gPublish) {
    if(result.empty()) {
      gPublish->publish(strConstTrainResponseChannelName, "{}",[&](const redisclient::RedisValue &) {
      });
    } else {
      gPublish->publish(strConstTrainResponseChannelName, result,[&](const redisclient::RedisValue &) {
    }
  }
}
