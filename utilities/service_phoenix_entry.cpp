#include <chrono>
#include <cinttypes>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
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

static const uint16_t iConstFetchAPIPortRangeMin = 41244;
static const uint16_t iConstFetchAPIPortRangeMax = 41254;
static void processText(const std::string &text);
std::shared_ptr<udp_server> gFetchTrainServer;

void train_phoenix_fetch_upd_main(void) {
  auto io_service = std::make_shared<boost::asio::io_service>();
  for (uint16_t port = iConstFetchAPIPortRangeMin;
       port < iConstFetchAPIPortRangeMax; port++) {
    try {
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port, "/watorvapor/wai.storage/conf/train.phoenix.fetch.api.json");
      gFetchTrainServer = std::make_shared<udp_server>(sock);
      gFetchTrainServer->start_receive(processText);
      DUMP_VAR(gFetchTrainServer.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}

void fetchOstrichTask(const string &lang);
void fetchParrotTask(const string &lang);
void fetchPhoenixTask(const string &lang);

void fetchOstrichSummary(void);
void fetchParrotSummary(void);
void fetchPhoenixSummary(void);

void processText(const std::string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto typeOpt = configJson.get_optional<string>("type");
    auto langOpt = configJson.get_optional<string>("lang");
    if (langOpt && typeOpt) {
      auto lang = langOpt.get();
      DUMP_VAR(lang);
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
    auto summaryOpt = configJson.get_optional<string>("summary");
    if (summaryOpt && typeOpt) {
      auto type = typeOpt.get();
      DUMP_VAR(type);
      if (type == "ostrich") {
        fetchOstrichSummary();
        return;
      }
      if (type == "parrot") {
        fetchParrotSummary();
        return;
      }
      if (type == "phoenix") {
        fetchPhoenixSummary();
        return;
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}

static const uint16_t iConstSaveAPIPortRangeMin = 41304;
static const uint16_t iConstSaveAPIPortRangeMax = 41314;
static void processText2(const std::string &text);
std::shared_ptr<udp_server> gSaveTrainServer;

void train_phoenix_save_upd_main(void) {
  // DUMP_VAR("train_save_upd_main");
  auto io_service = std::make_shared<boost::asio::io_service>();
  // DUMP_VAR("train_save_upd_main");
  for (uint16_t port = iConstSaveAPIPortRangeMin;
       port < iConstSaveAPIPortRangeMax; port++) {
    try {
      // DUMP_VAR(port);
      auto ep =
          std::make_shared<udp::endpoint>(address::from_string("::1"), port);
      auto sock = std::make_shared<udp::socket>(*io_service, *ep);
      DUMP_VAR(port);
      savePort(port, "/watorvapor/wai.storage/conf/train.phoenix.save.api.json");
      gSaveTrainServer = std::make_shared<udp_server>(sock);
      gSaveTrainServer->start_receive(processText2);
      DUMP_VAR(gSaveTrainServer.get());
      io_service->run();
    } catch (boost::exception &e) {
      DUMP_VAR(boost::diagnostic_information(e));
    }
  }
}

void saveOstrichTask(const string &lang, const string &url, const string &word);
void saveParrotTask(const string &lang, const string &url, const string &word);
void savePhoenixTask(const string &lang, const string &url, const string &word);

void processText2(const std::string &text) {
  try {
    pt::ptree configJson;
    std::stringstream ss;
    ss << text;
    pt::read_json(ss, configJson);
    auto langOpt = configJson.get_optional<string>("lang");
    auto urlOpt = configJson.get_optional<string>("url");
    auto wordOpt = configJson.get_optional<string>("word");
    if (langOpt && urlOpt && wordOpt) {
      auto lang = langOpt.get();
      auto url = urlOpt.get();
      auto word = wordOpt.get();
      DUMP_VAR(lang);
      DUMP_VAR(url);
      auto typeOpt = configJson.get_optional<string>("type");
      if (typeOpt) {
        auto type = typeOpt.get();
        DUMP_VAR(type);
        if (type == "ostrich") {
          saveOstrichTask(lang, url, word);
          return;
        }
        if (type == "parrot") {
          saveParrotTask(lang, url, word);
          return;
        }
        if (type == "phoenix") {
          savePhoenixTask(lang, url, word);
          return;
        }
      }
    }
  } catch (boost::exception &e) {
    DUMP_VAR(boost::diagnostic_information(e));
  }
}
