#include <chrono>
#include <cinttypes>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>
using namespace std;

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "log.hpp"

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
#include <boost/algorithm/string.hpp>

#include "udp_entry.hpp"
extern std::shared_ptr<udp_server> gSaveTrainServer;

#define EXTERN_DECLARE_DB(stage)                              \
  extern std::shared_ptr<URLStorage> gCNDone##stage##Storage; \
  extern std::shared_ptr<URLStorage> gCNTodo##stage##Storage; \
  extern std::shared_ptr<URLStorage> gJADone##stage##Storage; \
  extern std::shared_ptr<URLStorage> gJATodo##stage##Storage; \
  extern std::shared_ptr<DictionaryStorage> gCN##stage##Dict; \
  extern std::shared_ptr<DictionaryStorage> gJA##stage##Dict;

#include "dictstorage.hpp"
#include "urlstorage.hpp"
EXTERN_DECLARE_DB(Ostrich);
EXTERN_DECLARE_DB(Parrot);
EXTERN_DECLARE_DB(Phoenix);

string sha1(const string &data);

template <typename T>
void eachWord(const string &word, T fn) {
  list<string> list_string;
  string delim("{};");
  boost::split(list_string, word, boost::is_any_of(delim),
               boost::algorithm::token_compress_on);
  TRACE_VAR(list_string.size());
  for (auto wordPair : list_string) {
    TRACE_VAR(wordPair);
    if (wordPair.empty() == false) {
      string delim2(",");
      list<string> list_words;
      boost::split(list_words, wordPair, boost::is_any_of(delim2),
                   boost::algorithm::token_compress_on);
      if (list_words.size() == 2) {
        auto key = list_words.front();
        auto val = list_words.back();
        try {
          auto counter = std::stoi(val);
          if (counter > 0) {
            fn(key, counter);
          }
        } catch(const std::exception & e) {
          DUMP_VAR(e.what());
        }
      }
    }
  }
};

static bool markOstrich(const string &url, const string &lang) {
  bool ret = true;
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  if (lang == "cn") {
    if (gCNDoneOstrichStorage->is_has(doneName)) {
      ret = false;
    } else {
      gCNDoneOstrichStorage->add(doneName, url);
    }
    gCNTodoOstrichStorage->remove(doneName);
  } else if (lang == "ja") {
    if (gJADoneOstrichStorage->is_has(doneName)) {
      ret = false;
    } else {
      gJADoneOstrichStorage->add(doneName, url);
    }
    gJATodoOstrichStorage->remove(doneName);
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markOstrich_ms = end - start;
  DUMP_VAR(markOstrich_ms.count());
  return ret;
}

void saveOstrichTask(const string &lang, const string &url,
                     const string &word) {
  TRACE_VAR(lang, url, word);
  markOstrich(url, lang);
  auto save = [&](const string &key, int counter) {
    if (lang == "cn") {
      gCNOstrichDict->putWord(key, counter);
    } else if (lang == "ja") {
      gJAOstrichDict->putWord(key, counter);
    } else {
    }
  };
  eachWord(word, save);
  gSaveTrainServer->send("success");
}

static bool markParrot(const string &url, const string &lang) {
  bool ret = true;
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  if (lang == "cn") {
    if (gCNDoneParrotStorage->is_has(doneName)) {
      ret = false;
    } else {
      gCNDoneParrotStorage->add(doneName, url);
    }
    gCNTodoParrotStorage->remove(doneName);
  } else if (lang == "ja") {
    if (gJADoneParrotStorage->is_has(doneName)) {
      ret = false;
    } else {
      gJADoneParrotStorage->add(doneName, url);
    }
    gJATodoParrotStorage->remove(doneName);
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markOstrich_ms = end - start;
  DUMP_VAR(markOstrich_ms.count());
  return ret;
}

void saveParrotTask(const string &lang, const string &url, const string &word) {
  TRACE_VAR(lang, url, word);
  markParrot(url, lang);
  auto save = [&](const string &key, int counter) {
    if (lang == "cn") {
      gCNParrotDict->putWord(key, counter);
    } else if (lang == "ja") {
      gJAParrotDict->putWord(key, counter);
    } else {
    }
  };
  eachWord(word, save);
  gSaveTrainServer->send("success");
}


static bool markPhoenix(const string &url, const string &lang) {
  bool ret = true;
  auto start = std::chrono::system_clock::now();
  auto doneName = sha1(url);
  DUMP_VAR2(doneName, url);
  if (lang == "cn") {
    if (gCNDonePhoenixStorage->is_has(doneName)) {
      ret = false;
    } else {
      gCNDonePhoenixStorage->add(doneName, url);
    }
    gCNTodoPhoenixStorage->remove(doneName);
  } else if (lang == "ja") {
    if (gJADonePhoenixStorage->is_has(doneName)) {
      ret = false;
    } else {
      gJADonePhoenixStorage->add(doneName, url);
    }
    gJATodoPhoenixStorage->remove(doneName);
  } else {
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> markPhoenix_ms = end - start;
  DUMP_VAR(markPhoenix_ms.count());
  return ret;
}

void savePhoenixTask(const string &lang, const string &url,
                     const string &word) {
  TRACE_VAR(lang, url, word);
  markPhoenix(url, lang);
  auto save = [&](const string &key, int counter) {
    if (lang == "cn") {
      gCNPhoenixDict->putWord(key, counter);
    } else if (lang == "ja") {
      gJAPhoenixDict->putWord(key, counter);
    } else {
    }
  };
  eachWord(word, save);
  gSaveTrainServer->send("success");
}

void train_ostrich_word_write(void) {}
