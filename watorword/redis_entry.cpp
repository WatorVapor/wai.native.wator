#include "redis_entry.hpp"
#include "log.hpp"

const string strConstTrainChannelName("wai.train");

void redis_main(void) {
  try{
    boost::asio::ip::address address = boost::asio::ip::address::from_string("master.redis.wator.xyz");
    const unsigned short port = 6379;
    boost::asio::ip::tcp::endpoint endpoint(address, port);
    DUMP_VAR(endpoint);
    boost::asio::io_service ioService;
    RedisEntryClient client(ioService);
    redisclient::RedisAsyncClient publisher(ioService);
    publisher.connect(endpoint, [&](boost::system::error_code ec) {
      if(ec) {
        DUMP_VAR(ec);
      } else {
      }
    });
    redisclient::RedisAsyncClient subscriber(ioService);
    subscriber.connect(endpoint, [&](boost::system::error_code ec){
      if(ec) {
        DUMP_VAR(ec);
      } else {
        subscriber.subscribe(strConstTrainChannelName,std::bind(&RedisEntryClient::onMessageAPI, &client, std::placeholders::_1));
      }
    });  
    ioService.run();
  } catch(std::exception e) {
    DUMP_VAR(e.what());
  }
}
void RedisEntryClient::onMessageAPI(const std::vector<char> &buf) {
  string msg(buf.begin(),buf.end());
  DUMP_VAR(msg);
}
