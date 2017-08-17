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

string fetchOstrichTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
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
  gCNDoneOstrichStorage->openDB();
  gCNTodoOstrichStorage->openDB();
  gJADoneOstrichStorage->openDB();
  gJATodoOstrichStorage->openDB();
  
  while (true) {
    findTodo();
    DUMP_VAR(gOstrichTodoCN.size());
    DUMP_VAR(gOstrichTodoJA.size());
    std::unique_lock<std::mutex> lk(gTodoCvMutex);
    gTodoCV.wait(lk);
  }
  gCNDoneOstrichStorage->writeDB();
  gCNTodoOstrichStorage->writeDB();
  gJADoneOstrichStorage->writeDB();
  gJATodoOstrichStorage->writeDB();
  
  gCNDoneOstrichStorage->closeDB();
  gCNTodoOstrichStorage->closeDB();
  gJADoneOstrichStorage->closeDB();
  gJATodoOstrichStorage->closeDB();
  END_DB(Ostrich);
}

