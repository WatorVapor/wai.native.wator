#include "udp_entry.hpp"
#include "log.hpp"



udp_server::udp_server(shared_ptr<udp::socket> sock) : socket_(sock) { start_receive(); }
void udp_server::send(const std::string &msg) {
  boost::shared_ptr<std::string> message(new std::string(msg));
  socket_->async_send_to(
      boost::asio::buffer(*message), remote_endpoint_,
      boost::bind(&udp_server::handle_send, this, message));
}

void udp_server::start_receive() {
  socket_->async_receive_from(
      boost::asio::buffer(recv_buffer_), remote_endpoint_,
      boost::bind(&udp_server::handle_receive, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_receive(const boost::system::error_code &error,
                    std::size_t bytes_transferred) {
  auto start = std::chrono::system_clock::now();
  TRACE_VAR(remote_endpoint_);
  TRACE_VAR(bytes_transferred);
  std::string recv_str(recv_buffer_.data(), bytes_transferred);
  TRACE_VAR(recv_str);
  func_(recv_str);
  if (!error || error == boost::asio::error::message_size) {
    start_receive();
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> fp_ms = end - start;
  DUMP_VAR(fp_ms.count());
}

void udp_server::handle_send(boost::shared_ptr<std::string> msg) {
  DUMP_VAR(msg);
  DUMP_VAR(*msg);
}

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

void savePort(uint16_t port,const std::string &conf) {
  try {
    pt::ptree portConf;
    portConf.put("port", port);
    pt::write_json(conf, portConf);
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

