#include <condition_variable>
#include <mutex>
#include <vector>
using namespace std;

static vector<string> gVectTodoPathCN;
static vector<string> gVectTodoPathJA;
static std::mutex gVectoPathMutex;
static std::condition_variable gVectoPathCV;
static std::mutex gVectoPathCvMutex;

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

static void findToduURLsCN(void) {
  try {
    gCNTodoStorage->gets(iConstPathCacheMax,gVectTodoPathCN);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  // add seed
  if (gVectTodoPathCN.empty()) {
    gVectTodoPathCN.push_back(
        "https://zh.wikipedia.org/zh-cn/%E7%94%B5%E5%AD%90");
  }
}

static void findToduURLsJA(void) {
  try {
    gJATodoStorage->gets(iConstPathCacheMax,gVectTodoPathJA);
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  if (gVectTodoPathJA.empty()) {
    gVectTodoPathJA.push_back(
        "https://ja.wikipedia.org/wiki/%E9%9B%BB%E5%AD%90");
  }
}


static void findTodoURLs(void) {
  if (gVectTodoPathCN.empty()) {
    findToduURLsCN();
  }
  if (gVectTodoPathJA.empty()) {
    findToduURLsJA();
  }
}

#define START_DB(x) \
{\
  gCNDone##xStorage->openDB();\
  gCNTodo##xStorage->openDB();\
  gJADone##xStorage->openDB();\
  gJATodo##xStorage->openDB();\
}
#define END_DB(x) \
{\
  gCNDone##xStorage->closeDB();\
  gCNTodo##xStorage->closeDB();\
  gJADone##xStorage->closeDB();\
  gJATodo##xStorage->closeDB();\
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
    findTodoURLs();
    DUMP_VAR(gVectTodoPathCN.size());
    DUMP_VAR(gVectTodoPathJA.size());
    std::unique_lock<std::mutex> lk(gVectoPathCvMutex);
    gVectoPathCV.wait(lk);
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

