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
extern std::shared_ptr<udp_server> gSaveTrainServer;

#include "urlstorage.hpp"
extern std::shared_ptr<URLStorage> gCNDoneOstrichStorage;
extern std::shared_ptr<URLStorage> gCNTodoOstrichStorage;
extern std::shared_ptr<URLStorage> gJADoneOstrichStorage;
extern std::shared_ptr<URLStorage> gJATodoOstrichStorage;


#include "dictstorage.hpp"
std::shared_ptr<DictionaryStorage> gCNOstrichDict;
std::shared_ptr<DictionaryStorage> gJAOstrichDict;


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
  DUMP_VAR3(lang,url,word);
	list<string> list_string;
	string delim("{};");
	boost::split(list_string, word, boost::is_any_of(delim),
	             boost::algorithm::token_compress_on);
	DUMP_VAR(list_string.size());
	for (auto word : list_string) {
	  DUMP_VAR(word);
	  if (word.empty() == false) {
	  }
	}
  gSaveTrainServer->send("success");
}
void saveParrotTask(const string &lang,const string &url,const string &word) {
  gSaveTrainServer->send("success");
}
void savePhoenixTask(const string &lang,const string &url,const string &word) {
  gSaveTrainServer->send("success");
}

void train_word_write(void) {
  gCNOstrichDict = std::make_shared<DictionaryStorage>("/watorvapor/wai.storage/train/ostrich/dict/cn");
  gJAOstrichDict = std::make_shared<DictionaryStorage>("/watorvapor/wai.storage/train/ostrich/dict/ja");
}

