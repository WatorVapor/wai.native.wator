#include "redis_entry.hpp"
#include "log.hpp"

const string strConstTrainChannelName("wai.train");

void redis_main(void) {
  try{
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::resolver::query query("master.redis.wator.xyz", "6379");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = iter->endpoint();
    DUMP_VAR(endpoint);
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
