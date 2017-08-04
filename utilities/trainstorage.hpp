#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class TrainTaskStorage {
public:
  TrainTaskStorage();
  ~TrainTaskStorage();
private:
  void open(void);
  void close(void);
private:
  leveldb::DB *master_todo_db_;
  leveldb::DB *ostrich_done_db_;
  leveldb::DB *parrot_done_db_;
  leveldb::DB *phoenix_done_db_;
};
