#pragma once
#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
using namespace std;

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/all.hpp>
using namespace boost::asio::ip;


class udp_server {
public:
  udp_server(shared_ptr<udp::socket> sock);
  void send(const std::string &msg);
  void start_receive(std::function<void (const std::string&)> fn);
private:
  void start_receive();
  void handle_receive(const boost::system::error_code &error,
                      std::size_t bytes_transferred);
  void handle_send(boost::shared_ptr<std::string> msg);
public:
private:
  static const uint32_t iConstMSGBufferMax = 20 * 1024 * 1024;
  shared_ptr<udp::socket> socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, iConstMSGBufferMax> recv_buffer_;
  std::function<void (const std::string&)> func_;
};

void savePort(uint16_t port,const std::string &conf);
