#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "log.hpp"


class IpfsTextPump {
 public:
  IpfsTextPump();
  ~IpfsTextPump();
  template <typename T> void eachText(T fn) {
    while (true) {
      json task;
      string content;
      string ws;
      if (this->fetchMasterTask(task, content)) {
        fn(task, content,ws_);
      }
    }
  }
 private:
  bool fetchMasterTask(json &task, string &content);
  bool fetchBlockResource(void);
  string fetchIpfsResource(const string &cid);
  void parseResourceBlock(const string &block);
 private:
  string ws_;
  const string url_ = "https://ipfs.wator.xyz/ipfs";
  vector<string> resoureCIDs_ = {};
  string group_;
  string task_;
  bool isRunBlock = false;
  int iTototl = 0;
  int iProccessed = 0;
};
