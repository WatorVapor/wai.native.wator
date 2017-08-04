#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class TrainTaskStorage {
public:
  TrainTaskStorage(const string &path, const string &prefix);
  ~TrainTaskStorage();
private:
  void open();
  void close();
private:
  string out_db_path_;
  string iter_db_path_;
};
