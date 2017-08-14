#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class URLStorage {
public:
  URLStorage(const string &path);
  ~URLStorage();
  void openDB();
  void closeDB();
  void writeDB();
  void dumpSnapshotDB();
private:
private:
  string out_db_path_;
  string iter_db_path_;
  leveldb::WriteBatch gSaveDBBatch;
};
