#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
using namespace std;

#include "log.hpp"
#include "udp_entry.hpp"

static vector<string> gOstrichTodoCN;
static vector<string> gOstrichTodoJA;

static std::mutex gTodoMutex;
static std::condition_variable gTodoCV;
static std::mutex gTodoCvMutex;

static int iConstPathCacheMax = 32;

extern std::shared_ptr<udp_server> gFetchTrainServer;

void fetchOstrichTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gOstrichTodoCN.empty()) {
      gTodoCV.notify_all();
      gFetchTrainServer->send("");
    } else {
      auto top = gOstrichTodoCN.back();
      DUMP_VAR(top);
      gOstrichTodoCN.pop_back();
      DUMP_VAR(gOstrichTodoCN.size());
      gFetchTrainServer->send(top);
    }
  } else if (lang == "ja") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gOstrichTodoJA.empty()) {
      gTodoCV.notify_all();
      gFetchTrainServer->send("");
    } else {
      auto top = gOstrichTodoJA.back();
      DUMP_VAR(top);
      gOstrichTodoJA.pop_back();
      DUMP_VAR(gOstrichTodoJA.size());
      gFetchTrainServer->send(top);
    }
  } else {
  }
}
void fetchParrotTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
}
void fetchPhoenixTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
}

#include "urlstorage.hpp"
std::shared_ptr<URLStorage> gCNDoneOstrichStorage;
std::shared_ptr<URLStorage> gCNTodoOstrichStorage;
std::shared_ptr<URLStorage> gJADoneOstrichStorage;
std::shared_ptr<URLStorage> gJATodoOstrichStorage;

#include "dictstorage.hpp"
std::shared_ptr<DictionaryStorage> gCNOstrichDict;
std::shared_ptr<DictionaryStorage> gJAOstrichDict;

static void findOstrichTodoCN(void) {
  try {
    gCNTodoOstrichStorage->gets(iConstPathCacheMax, gOstrichTodoCN);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
}

static void findOstrichTodoJA(void) {
  try {
    gJATodoOstrichStorage->gets(iConstPathCacheMax, gOstrichTodoJA);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
}

static void findTodo(void) {
  if (gOstrichTodoCN.empty()) {
    findOstrichTodoCN();
  }
  if (gOstrichTodoJA.empty()) {
    findOstrichTodoJA();
  }
}

#define START_DB(x)                                                            \
  {                                                                            \
    gCNDone##x##Storage->openDB();                                             \
    gCNTodo##x##Storage->openDB();                                             \
    gJADone##x##Storage->openDB();                                             \
    gJATodo##x##Storage->openDB();                                             \
    gCN##x##Dict->openDB();                                                    \
    gJA##x##Dict->openDB();                                                    \
  }
#define END_DB(x)                                                              \
  {                                                                            \
    gCNDone##x##Storage->writeDB();                                            \
    gCNTodo##x##Storage->writeDB();                                            \
    gJADone##x##Storage->writeDB();                                            \
    gJATodo##x##Storage->writeDB();                                            \
                                                                               \
    gCNDone##x##Storage->closeDB();                                            \
    gCNTodo##x##Storage->closeDB();                                            \
    gJADone##x##Storage->closeDB();                                            \
    gJATodo##x##Storage->closeDB();                                            \
    gCN##x##Dict->closeDB();                                                   \
    gJA##x##Dict->closeDB();                                                   \
  }

void train_collect(void) {
  gCNDoneOstrichStorage = std::make_shared<URLStorage>(
      "/watorvapor/wai.storage/train/ostrich/done/cn");
  gCNTodoOstrichStorage = std::make_shared<URLStorage>(
      "/watorvapor/wai.storage/train/ostrich/todo/cn");
  gJADoneOstrichStorage = std::make_shared<URLStorage>(
      "/watorvapor/wai.storage/train/ostrich/done/ja");
  gJATodoOstrichStorage = std::make_shared<URLStorage>(
      "/watorvapor/wai.storage/train/ostrich/todo/ja");
  gCNOstrichDict = std::make_shared<DictionaryStorage>(
      "/watorvapor/wai.storage/train/ostrich/dict/cn");
  gJAOstrichDict = std::make_shared<DictionaryStorage>(
      "/watorvapor/wai.storage/train/ostrich/dict/ja");

  START_DB(Ostrich);

  while (true) {
    findTodo();
    DUMP_VAR(gOstrichTodoCN.size());
    DUMP_VAR(gOstrichTodoJA.size());
    std::unique_lock<std::mutex> lk(gTodoCvMutex);
    gTodoCV.wait(lk);
  }
  END_DB(Ostrich);
}

void fetchOstrichSummary(void) {
  std::string summary;
  summary += gCNDoneOstrichStorage->summary();
  summary += "\n";
  summary += gCNTodoOstrichStorage->summary();
  ;
  summary += "\n";
  summary += gJADoneOstrichStorage->summary();
  ;
  summary += "\n";
  summary += gJATodoOstrichStorage->summary();
  ;
  summary += "\n";
  summary += gCNOstrichDict->summary();
  ;
  summary += "\n";
  summary += gJAOstrichDict->summary();
  ;
  gFetchTrainServer->send(summary);
}
void fetchParrotSummary(void) {}
void fetchPhoenixSummary(void) {}
