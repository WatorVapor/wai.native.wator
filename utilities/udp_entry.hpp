#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/all.hpp>


class udp_server {
using namespace boost::asio::ip;
public:
  udp_server(shared_ptr<udp::socket> sock) : socket_(sock) { start_receive(); }
  void send(const std::string &msg) {
    boost::shared_ptr<std::string> message(new std::string(msg));
    socket_->async_send_to(
        boost::asio::buffer(*message), remote_endpoint_,
        boost::bind(&udp_server::handle_send, this, message));
  }

private:
  void start_receive() {
    socket_->async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code &error,
                      std::size_t bytes_transferred) {
    auto start = std::chrono::system_clock::now();
    TRACE_VAR(remote_endpoint_);
    TRACE_VAR(bytes_transferred);
    std::string recv_str(recv_buffer_.data(), bytes_transferred);
    TRACE_VAR(recv_str);
    processText(recv_str);
    if (!error || error == boost::asio::error::message_size) {
      start_receive();
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - start;
    DUMP_VAR(fp_ms.count());
  }

  void handle_send(boost::shared_ptr<std::string> msg) {
    DUMP_VAR(msg);
    DUMP_VAR(*msg);
  }

  shared_ptr<udp::socket> socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, iConstMSGBufferMax> recv_buffer_;
};

