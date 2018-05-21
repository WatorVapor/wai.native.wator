#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include "log.hpp"


class IpfsTextPump {
 public:
  IpfsTextPump();
  ~IpfsTextPump();
  template <typename T> void eachText(T fn) {
    while (true) {
      pt::ptree task;
      string content;
      string ws;
      if (this->fetchMasterTask(task, content)) {
        fn(task, content,ws_);
      }
    }
  }
 private:
  bool fetchMasterTask(pt::ptree &task, string &content);
 private:
  string ws_;
  const string url_ = "https://ipfs.wator.xyz/ipfs";
  string resoureBlock_;
  int iTototl = 0;
  int iProccessed = 0;
};
