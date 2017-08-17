#include <condition_variable>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

#include "log.hpp"

static vector<string> gOstrichTodoCN;
static vector<string> gOstrichTodoJA;

static std::mutex gTodoMutex;
static std::condition_variable gTodoCV;
static std::mutex gTodoCvMutex;

static int iConstPathCacheMax = 32;

extern std::shared_ptr<udp_server> gFetchServer;

string fetchOstrichTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gOstrichTodoCN.empty()) {
      gTodoCV.notify_all();
      gFetchServer->send("");
    } else {
      auto top = gOstrichTodoCN.back();
      DUMP_VAR(top);
      gOstrichTodoCN.pop_back();
      DUMP_VAR(gOstrichTodoCN.size());
      gFetchServer->send(top);
    }
  } else if (lang == "ja") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gOstrichTodoJA.empty()) {
      gTodoCV.notify_all();
      gFetchServer->send("");
    } else {
      auto top = gOstrichTodoJA.back();
      DUMP_VAR(top);
      gOstrichTodoJA.pop_back();
      DUMP_VAR(gOstrichTodoJA.size());
      gFetchServer->send(top);
    }
  } else {
  }
  return "";
}
string fetchParrotTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
  return "";
}
string fetchPhoenixTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
  return "";
}

#include "urlstorage.hpp"
std::shared_ptr<URLStorage> gCNDoneOstrichStorage;
std::shared_ptr<URLStorage> gCNTodoOstrichStorage;
std::shared_ptr<URLStorage> gJADoneOstrichStorage;
std::shared_ptr<URLStorage> gJATodoOstrichStorage;

static void findOstrichTodoCN(void) {
  try {
    gCNTodoOstrichStorage->gets(iConstPathCacheMax,gOstrichTodoCN);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
}

static void findOstrichTodoJA(void) {
  try {
    gJATodoOstrichStorage->gets(iConstPathCacheMax,gOstrichTodoJA);
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

#define START_DB(x) \
{\
  gCNDone ## x ## Storage->openDB();\
  gCNTodo ## x ## Storage->openDB();\
  gJADone ## x ## Storage->openDB();\
  gJATodo ## x ## Storage->openDB();\
}
#define END_DB(x) \
{\
  gCNDone ## x ## Storage->writeDB();\
  gCNTodo ## x ## Storage->writeDB();\
  gJADone ## x ## Storage->writeDB();\
  gJATodo ## x ## Storage->writeDB();\
  \
  gCNDone ## x ## Storage->closeDB();\
  gCNTodo ## x ## Storage->closeDB();\
  gJADone ## x ## Storage->closeDB();\
  gJATodo ## x ## Storage->closeDB();\
}


void train_collect(void) {
  gCNDoneOstrichStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/train/ostrich/done/cn");
  gCNTodoOstrichStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/train/ostrich/todo/cn");
  gJADoneOstrichStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/train/ostrich/done/ja");
  gJATodoOstrichStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/train/ostrich/todo/ja");
  
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

