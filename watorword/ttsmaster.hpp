#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>

class TTSMaster {
 public:
  TTSMaster(const string &path);
  ~TTSMaster();
  string getPhoneme(const string &pinyin) const;
  string getHanzi(const string &hanzi) const;
  string getPhrase(const string &word) const;
 
 private:
  void openDB();
  void closeDB();
 private:
  string master_db_path_phrase_;
  string master_db_path_hanzi_;
  string master_db_path_phoneme_;
  leveldb::DB * master_phrase_;
  leveldb::DB * master_hanzi_;
  leveldb::DB * master_phoneme_;
};
