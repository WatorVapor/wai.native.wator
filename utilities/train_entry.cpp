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
using namespace boost::asio::ip;

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;


#include "log.hpp"
#include "udp_entry.hpp"

static const uint16_t iConstFetchAPIPortRangeMin = 41284;
static const uint16_t iConstFetchAPIPortRangeMax = 41294;
static void processText(const std::string &text);
std::shared_ptr<udp_server> gFetchServer;



void train_fetch_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstFetchAPIPortRangeMin; port < iConstFetchAPIPortRangeMax;
       port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port,"/watorvapor/wai.storage/conf/train.fetch.api.json");
      gFetchServer = std::make_shared<udp_server>(sock);
      gFetchServer->start_receive(processText);
      DUMP_VAR(gFetchServer.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}

string fetchOstrichTask(const string &lang);
string fetchParrotTask(const string &lang);
string fetchPhoenixTask(const string &lang);

void processText(const std::string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      auto lang = langOpt.get();
      DUMP_VAR(lang);
      auto typeOpt = configJson.get_optional<string>("type");
      if (typeOpt) {
        auto type = typeOpt.get();
        DUMP_VAR(type);
        if (type == "ostrich") {
          fetchOstrichTask(lang);
          return;
        }
        if (type == "parrot") {
          fetchParrotTask(lang);
          return;
        }
        if (type == "phoenix") {
          fetchPhoenixTask(lang);
          return;
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

static const uint16_t iConstSaveAPIPortRangeMin = 41304;
static const uint16_t iConstSaveAPIPortRangeMax = 41314;
static void processText2(const std::string &text);
std::shared_ptr<udp_server> gSaveServer;

void train_save_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstSaveAPIPortRangeMin; port < iConstSaveAPIPortRangeMin;
       port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port,"/watorvapor/wai.storage/conf/train.save.api.json");
      gSaveServer = std::make_shared<udp_server>(sock);
      gSaveServer->start_receive(processText2);
      DUMP_VAR(gSaveServer.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}


void saveOstrichTask(const string &lang);
void saveParrotTask(const string &lang);
void savePhoenixTask(const string &lang);

void processText2(const std::string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt) {
      auto lang = langOpt.get();
      DUMP_VAR(lang);
      auto typeOpt = configJson.get_optional<string>("type");
      if (typeOpt) {
        auto type = typeOpt.get();
        DUMP_VAR(type);
        if (type == "ostrich") {
          saveOstrichTask(lang);
          return;
        }
        if (type == "parrot") {
          saveParrotTask(lang);
          return;
        }
        if (type == "phoenix") {
          savePhoenixTask(lang);
          return;
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

