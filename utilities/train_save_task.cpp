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

string sha1(const string &data);


static void markOstrich(const string &url,const string &lang) {
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  if(lang == "cn") {
    gCNDoneOstrichStorage->add(doneName,url);
    gCNTodoOstrichStorage->remove(doneName);
  } else if(lang == "ja") {
    gJADoneOstrichStorage->add(doneName,url);
    gJATodoOstrichStorage->remove(doneName);
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markOstrich_ms = end - start;
  DUMP_VAR(markOstrich_ms.count());
}



void saveOstrichTask(const string &lang,const string &url,const string &word) {
  //markOstrich(url,lang);
  gSaveServer->send("success");
}
void saveParrotTask(const string &lang,const string &url,const string &word) {
  gSaveServer->send("success");
}
void savePhoenixTask(const string &lang,const string &url,const string &word) {
  gSaveServer->send("success");
}

void train_word_write(void) {
}

