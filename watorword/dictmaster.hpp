#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class DictionaryMaster {
 public:
  DictionaryMaster(const string &path, const string &prefix);
  DictionaryMaster(const string &path);
  ~DictionaryMaster();
  void openDB();
  void closeDB();
  void writeDB();
  void dumpSnapshotDB();
  void putWord(const string &word, int counter);
  double getDoublePred(const string &word);

 private:
 private:
  string out_db_path_;
  string iter_db_path_;
  leveldb::WriteBatch gSaveDBBatch;
};
