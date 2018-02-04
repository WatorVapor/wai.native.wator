#pragma once
#include <chrono>
#include <cinttypes>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
using namespace std;

#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

class RedisClient
{
public:
    RedisClient(boost::asio::io_service &ioService)
        : ioService(ioService)
    {}
    void onMessageAPI(const std::vector<char> &buf);
private:
    boost::asio::io_service &ioService;
};
