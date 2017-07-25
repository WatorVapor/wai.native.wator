#include <string>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;
#include "log.hpp"

class WikiCrawler {
public:
  WikiCrawler();
  ~WikiCrawler();
private:
private:
};
