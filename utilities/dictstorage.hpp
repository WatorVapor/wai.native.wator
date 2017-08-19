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
  string out_db_path_;
  string iter_db_path_;
  leveldb::DB *save_;
  leveldb::WriteBatch gSaveDBBatch;
};
