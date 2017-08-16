#include <iostream>
#include <string>
#include <thread>
#include <fstream>
using namespace std;

#include "log.hpp"

#include "urlstorage.hpp"
std::shared_ptr<URLStorage> gCNMasterStorage;
std::shared_ptr<URLStorage> gCNTodoStorage;
std::shared_ptr<URLStorage> gJAMasterStorage;
std::shared_ptr<URLStorage> gJATodoStorage;




int main(int ac, char *av[]) {
  
  gCNMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/master");
  gCNTodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/todo");
  gJAMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/master");
  gJATodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/todo");
  
  gCNMasterStorage->openDB();
  gCNTodoStorage->openDB();
  gJAMasterStorage->openDB();
  gJATodoStorage->openDB();
  
  
  gCNMasterStorage->closeDB();
  gCNTodoStorage->closeDB();
  gJAMasterStorage->closeDB();
  gJATodoStorage->closeDB();
  
  return 0;
}
