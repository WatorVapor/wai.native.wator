#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>

class DictionaryMaster {
 public:
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
  string master_db_path_;
  leveldb::DB *master_;
  leveldb::DB *masterCast_;
};
