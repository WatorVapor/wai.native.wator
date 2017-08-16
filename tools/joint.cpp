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


std::shared_ptr<URLStorage> gCNJointStorage;
std::shared_ptr<URLStorage> gJAJointStorage;


int main(int ac, char *av[]) {
  
  gCNMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/master");
  gCNTodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/todo");
  gCNJointStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/cn/joint");

  gJAMasterStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/master");
  gJATodoStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/todo");
  gJAJointStorage = std::make_shared<URLStorage>("/watorvapor/wai.storage/leveldb/ja/joint");
  
  gCNMasterStorage->openDB();
  gCNTodoStorage->openDB();
  gCNJointStorage->openDB();
  
  gJAMasterStorage->openDB();
  gJATodoStorage->openDB();
  gJAJointStorage->openDB();  
  
  gCNMasterStorage->closeDB();
  gCNTodoStorage->closeDB();
  gCNJointStorage->closeDB();
  
  gJAMasterStorage->closeDB();
  gJATodoStorage->closeDB();
  gJAJointStorage->closeDB();
  
  return 0;
}
