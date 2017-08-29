#include <memory>
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
  void gets(int max, vector<std::string> &urls);
  void dumpSnapshotDB();
  void add(const std::string &key, const std::string &value);
  void remove(const std::string &key);
  bool is_has(const std::string &key);
  string summary(void);
  void copy(std::shared_ptr<URLStorage> dst);

 private:
 private:
  string db_path_;
  leveldb::DB *save_;
};
