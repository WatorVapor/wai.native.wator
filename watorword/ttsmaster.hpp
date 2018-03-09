#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>

class TTSMaster {
 public:
  TTSMaster(const string &path);
  ~TTSMaster();
  void openDB();
  void closeDB();
  string getPhoneme(const string &word);

 private:
 private:
  string master_db_path_;
  leveldb::DB *master_;
};
