#include <cinttypes>
#include <exception>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <list>
using namespace std;


#include <condition_variable>
#include <mutex>
#include <atomic>

#include "log.hpp"


#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include <boost/algorithm/string.hpp>


#include "udp_entry.hpp"
extern std::shared_ptr<udp_server> gSaveServer;


void saveOstrichTask(const string &lang) {
  gSaveServer->send("success");
}
void saveParrotTask(const string &lang) {
  gSaveServer->send("success");
}
void savePhoenixTask(const string &lang) {
  gSaveServer->send("success");
}

void train_word_write(void) {
}

