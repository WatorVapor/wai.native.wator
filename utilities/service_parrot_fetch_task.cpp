#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
using namespace std;

#include "log.hpp"
#include "udp_entry.hpp"

static vector<string> gParrotTodoCN;
static vector<string> gParrotTodoJA;

static std::mutex gTodoMutex;
static std::condition_variable gTodoCV;
static std::mutex gTodoCvMutex;

static int iConstPathCacheMax = 32;

extern std::shared_ptr<udp_server> gFetchTrainServer;

/*
void fetchOstrichTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
}
*/

void fetchParrotTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gParrotTodoCN.empty()) {
      gTodoCV.notify_all();
      gFetchTrainServer->send("");
    } else {
      auto top = gParrotTodoCN.back();
      DUMP_VAR(top);
      gParrotTodoCN.pop_back();
      DUMP_VAR(gParrotTodoCN.size());
      gFetchTrainServer->send(top);
    }
  } else if (lang == "ja") {
    std::lock_guard<std::mutex> lock(gTodoMutex);
    if (gParrotTodoJA.empty()) {
      gTodoCV.notify_all();
      gFetchTrainServer->send("");
    } else {
      auto top = gParrotTodoJA.back();
      DUMP_VAR(top);
      gParrotTodoJA.pop_back();
      DUMP_VAR(gParrotTodoJA.size());
      gFetchTrainServer->send(top);
    }
  } else {
  }
}
/*
void fetchPhoenixTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
}
*/


#include "dictstorage.hpp"
#include "urlstorage.hpp"
#define DECLARE_DB(stage)                              \
  std::shared_ptr<URLStorage> gCNDone##stage##Storage; \
  std::shared_ptr<URLStorage> gCNTodo##stage##Storage; \
  std::shared_ptr<URLStorage> gJADone##stage##Storage; \
  std::shared_ptr<URLStorage> gJATodo##stage##Storage; \
  std::shared_ptr<DictionaryStorage> gCN##stage##Dict; \
  std::shared_ptr<DictionaryStorage> gJA##stage##Dict;

DECLARE_DB(Parrot);

#define TRY_FIND_TASK(stage, lang)                                 \
  {                                                                \
    if (g##stage##Todo##lang.empty()) {                            \
      try {                                                        \
        if(g##lang##Todo##stage##Storage) {                         \
          g##lang##Todo##stage##Storage->gets(iConstPathCacheMax,    \
                                            g##stage##Todo##lang); \
        }                                                           \
      } catch (std::exception & e) {                               \
        DUMP_VAR(e.what());                                        \
      } catch (...) {                                              \
      }                                                            \
    }                                                              \
  }

static void findTodo(void) {
  TRY_FIND_TASK(Parrot, CN);
  TRY_FIND_TASK(Parrot, JA);
}

#define START_DB(stageN, stageP)                              \
  {                                                           \
    gCNDone##stageN##Storage = std::make_shared<URLStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/done/cn"); \
    gCNTodo##stageN##Storage = std::make_shared<URLStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/todo/cn"); \
    gJADone##stageN##Storage = std::make_shared<URLStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/done/ja"); \
    gJATodo##stageN##Storage = std::make_shared<URLStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/todo/ja"); \
    gCN##stageN##Dict = std::make_shared<DictionaryStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/dict/cn"); \
    gJA##stageN##Dict = std::make_shared<DictionaryStorage>(  \
        "/watorvapor/wai.storage/train/" #stageP "/dict/ja"); \
    gCNDone##stageN##Storage->openDB();                       \
    gCNTodo##stageN##Storage->openDB();                       \
    gJADone##stageN##Storage->openDB();                       \
    gJATodo##stageN##Storage->openDB();                       \
    gCN##stageN##Dict->openDB();                              \
    gJA##stageN##Dict->openDB();                              \
  }
#define END_DB(stage)                   \
  {                                     \
    gCNDone##stage##Storage->closeDB(); \
    gCNTodo##stage##Storage->closeDB(); \
    gJADone##stage##Storage->closeDB(); \
    gJATodo##stage##Storage->closeDB(); \
    gCN##stage##Dict->closeDB();        \
    gJA##stage##Dict->closeDB();        \
  }

void train_parrot_collect(void) {
  START_DB(Parrot, parrot);

  while (true) {
    findTodo();
    std::unique_lock<std::mutex> lk(gTodoCvMutex);
    gTodoCV.wait(lk);
  }
  END_DB(Parrot);
}

#define FETCH_SUMMARY(stage)                       \
  {                                                \
    if(gCNDone##stage##Storage) {\
      summary += gCNDone##stage##Storage->summary(); \
    }\
    summary += "\n";                               \
    if(gCNTodo##stage##Storage) {\
      summary += gCNTodo##stage##Storage->summary(); \
    }\
    ;                                              \
    summary += "\n";                               \
    if(gJADone##stage##Storage) {\
      summary += gJADone##stage##Storage->summary(); \
    }\
    ;                                              \
    summary += "\n";                               \
    if(gJATodo##stage##Storage) {\
      summary += gJATodo##stage##Storage->summary(); \
    }\
    ;                                              \
    summary += "\n";                               \
    if(gCN##stage##Dict) {\
      summary += gCN##stage##Dict->summary();        \
    }\
    ;                                              \
    summary += "\n";                               \
    if(gJA##stage##Dict) {\
      summary += gJA##stage##Dict->summary();        \
    }\
    ;                                              \
  }

/*
void fetchOstrichSummary(void) {
}
*/
void fetchParrotSummary(void) {
  std::string summary;
  FETCH_SUMMARY(Parrot);
  gFetchTrainServer->send(summary);
}
/*
void fetchPhoenixSummary(void) {}
*/
