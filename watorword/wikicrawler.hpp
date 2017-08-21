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

class WikiCrawler {
 public:
  WikiCrawler();
  ~WikiCrawler();
  void parse(const pt::ptree &task, string &content);

 private:
  void up(const pt::ptree &task, string &urls);

 private:
  const int iConstOnceUpURLMax = 20;
};
