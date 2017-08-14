#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class URLStorage {
public:
  URLStorage(const std::string &path);
  ~URLStorage();
  void openDB();
  void closeDB();
  void writeDB();
  void gets(int max,vector<std::string> &urls);
  void dumpSnapshotDB();
private:
private:
  string out_db_path_;
  string iter_db_path_;
  leveldb::DB *save_;
  leveldb::WriteBatch saveBatch_;
};
