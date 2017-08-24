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

static vector<string> gParrotTodoCN;
static vector<string> gParrotTodoJA;

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

std::shared_ptr<URLStorage> gCNDoneParrotStorage;
std::shared_ptr<URLStorage> gCNTodoParrotStorage;
std::shared_ptr<URLStorage> gJADoneParrotStorage;
std::shared_ptr<URLStorage> gJATodoParrotStorage;


#include "dictstorage.hpp"
std::shared_ptr<DictionaryStorage> gCNOstrichDict;
std::shared_ptr<DictionaryStorage> gJAOstrichDict;

std::shared_ptr<DictionaryStorage> gCNParrotDict;
std::shared_ptr<DictionaryStorage> gJAParrotDict;



#define TRY_FIND_TASK(stage,lang) \
{\
  if (g##stage##Todo##lang.empty()) {\
    try {\
      g##lang##Todo##stage##Storage->gets(iConstPathCacheMax, g##stage##Todo##lang);\
    } catch (std::exception &e) {\
      DUMP_VAR(e.what());\
    } catch (...) {\
    }\
  }\
}

static void findTodo(void) {
  TRY_FIND_TASK(Ostrich,CN);
  TRY_FIND_TASK(Ostrich,JA);
  TRY_FIND_TASK(Parrot,CN);
  TRY_FIND_TASK(Parrot,JA);
}

#define DECLARE_DB(stageN,stageP) \
{\
  gCNDone##stageN##Storage = std::make_shared<URLStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/done/cn");\
  gCNTodo##stageN##Storage = std::make_shared<URLStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/todo/cn");\
  gJADone##stageN##Storage = std::make_shared<URLStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/done/ja");\
  gJATodo##stageN##Storage = std::make_shared<URLStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/todo/ja");\
  gCN##stageN##Dict = std::make_shared<DictionaryStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/dict/cn");\
  gJA##stageN##Dict = std::make_shared<DictionaryStorage>(\
      "/watorvapor/wai.storage/train/"#stageP"/dict/ja");\
}

#define START_DB(x)                \
  {                                \
    gCNDone##x##Storage->openDB(); \
    gCNTodo##x##Storage->openDB(); \
    gJADone##x##Storage->openDB(); \
    gJATodo##x##Storage->openDB(); \
    gCN##x##Dict->openDB();        \
    gJA##x##Dict->openDB();        \
  }
#define END_DB(x)                   \
  {                                 \
    gCNDone##x##Storage->writeDB(); \
    gCNTodo##x##Storage->writeDB(); \
    gJADone##x##Storage->writeDB(); \
    gJATodo##x##Storage->writeDB(); \
                                    \
    gCNDone##x##Storage->closeDB(); \
    gCNTodo##x##Storage->closeDB(); \
    gJADone##x##Storage->closeDB(); \
    gJATodo##x##Storage->closeDB(); \
    gCN##x##Dict->closeDB();        \
    gJA##x##Dict->closeDB();        \
  }

void train_collect(void) {
  DECLARE_DB(Ostrich,ostrich);
  DECLARE_DB(Parrot,parrot);
  START_DB(Ostrich);
  START_DB(Parrot);
  
  
  while (true) {
    findTodo();
    DUMP_VAR(gOstrichTodoCN.size());
    DUMP_VAR(gOstrichTodoJA.size());
    std::unique_lock<std::mutex> lk(gTodoCvMutex);
    gTodoCV.wait(lk);
  }
  END_DB(Ostrich);
  END_DB(Parrot);
}

#define FETCH_SUMMARY(stage) \
{\
  summary += gCNDone##stage##Storage->summary();\
  summary += "\n";\
  summary += gCNTodo##stage##Storage->summary();\
  ;\
  summary += "\n";\
  summary += gJADone##stage##Storage->summary();\
  ;\
  summary += "\n";\
  summary += gJATodo##stage##Storage->summary();\
  ;\
  summary += "\n";\
  summary += gCN##stage##Dict->summary();\
  ;\
  summary += "\n";\
  summary += gJA##stage##Dict->summary();\
  ;\
}


void fetchOstrichSummary(void) {
  std::string summary;
  FETCH_SUMMARY(Ostrich);
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
void fetchParrotSummary(void) {
  std::string summary;
  FETCH_SUMMARY(Parrot);
 summary += gCNDoneParrotStorage->summary();
  summary += "\n";
  summary += gCNTodoParrotStorage->summary();
  ;
  summary += "\n";
  summary += gJADoneParrotStorage->summary();
  ;
  summary += "\n";
  summary += gJATodoParrotStorage->summary();
  ;
  summary += "\n";
  summary += gCNParrotDict->summary();
  ;
  summary += "\n";
  summary += gJAParrotDict->summary();
  ;
  gFetchTrainServer->send(summary);
}
void fetchPhoenixSummary(void) {}
