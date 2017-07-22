#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>


class DictionaryStorage {
public:
  DictionaryStorage(const string &path,const string &prefix);
  ~DictionaryStorage();
  void openDB();
  void closeDB();
  void writeDB();
  void dumpSnapshotDB();
  void putWord(const string &word,int counter);
private:
private:
  string out_db_path_;
  string iter_db_path_;
  leveldb::WriteBatch gSaveDBBatch;
};
