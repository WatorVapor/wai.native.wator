#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>

class TTSMaster {
 public:
  TTSMaster(const string &path);
  ~TTSMaster();
  string getPhoneme(const string &word);
 
 private:
  void openDB();
  void closeDB();
 private:
  string phrase_master_db_path_;
  string hanzi_master_db_path_;
  string phoneme_master_db_path_;
  leveldb::DB * phrase_master_;
  leveldb::DB * hanzi_master_;
  leveldb::DB * phoneme_master_;
};
