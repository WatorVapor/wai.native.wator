#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class DictionaryStorage {
 public:
  DictionaryStorage(const string &path);
  ~DictionaryStorage();
  void openDB();
  void closeDB();
  void putWord(const string &word, int counter);
  string summary(void);

 private:
 private:
  string db_path_;
  leveldb::DB *save_;
};
