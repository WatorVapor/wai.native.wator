#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/all.hpp>


class udp_server {
using namespace std;
using namespace boost::asio::ip;
public:
  udp_server(shared_ptr<udp::socket> sock);
  void send(const std::string &msg);
private:
  void start_receive();
  void handle_receive(const boost::system::error_code &error,
                      std::size_t bytes_transferred);
  void handle_send(boost::shared_ptr<std::string> msg);
public:
private:
  shared_ptr<udp::socket> socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, iConstMSGBufferMax> recv_buffer_;
};

