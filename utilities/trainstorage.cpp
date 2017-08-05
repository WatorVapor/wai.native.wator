#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/format.hpp>

#include "trainstorage.hpp"
#include "log.hpp"



TrainTaskStorage::TrainTaskStorage() {
}

TrainTaskStorage::~TrainTaskStorage() {}

#define OPEN_LEVELDB_CREATE(name,ptr)\
{\
  if (name == nullptr) {\
    leveldb::Options options;\
    options.create_if_missing = true;\
    options.max_open_files = 512;\
    options.paranoid_checks = true;\
    options.compression = leveldb::kNoCompression;\
    auto status = leveldb::DB::Open(options, name, &ptr);\
    if (status.ok() == false) {\
      DUMP_VAR(status.ToString());\
      ptr = nullptr;\
    }\
  }\
}
  

#define OPEN_LEVELDB_NO_CREATE(name,ptr)\
{\
  if (name == nullptr) {\
    leveldb::Options options;\
    options.create_if_missing = false;\
    options.max_open_files = 512;\
    options.paranoid_checks = true;\
    options.compression = leveldb::kNoCompression;\
    auto status = leveldb::DB::Open(options, name, &ptr);\
    if (status.ok() == false) {\
      DUMP_VAR(status.ToString());\
      ptr = nullptr;\
    }\
  }\
}

void TrainTaskStorage::open(void) {
  OPEN_LEVELDB_NO_CREATE("/watorvapor/wai.storage/train/master/cn",master_todo_cn_db_);
  OPEN_LEVELDB_NO_CREATE("/watorvapor/wai.storage/train/master/ja",master_todo_ja_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/ostrich/cn",ostrich_done_cn_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/parrot/cn",parrot_done_cn_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/phoenix/cn",phoenix_done_cn_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/ostrich/ja",ostrich_done_ja_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/parrot/ja",parrot_done_ja_db_);
  OPEN_LEVELDB_CREATE("/watorvapor/wai.storage/train/phoenix/ja",phoenix_done_ja_db_);
}

CLOSE_LEVELDB

#define CLOSE_LEVELDB(ptr)\
{\
  if (ptr != nullptr) {
    delete ptr;
    ptr = nullptr;
  }
}

void TrainTaskStorage::close(void) {
  CLOSE_LEVELDB(master_todo_cn_db_);
  CLOSE_LEVELDB(master_todo_ja_db_);
  CLOSE_LEVELDB(ostrich_done_cn_db_);
  CLOSE_LEVELDB(parrot_done_cn_db_);
  CLOSE_LEVELDB(phoenix_done_cn_db_);
  CLOSE_LEVELDB(ostrich_done_ja_db_);
  CLOSE_LEVELDB(parrot_done_ja_db_);
  CLOSE_LEVELDB(phoenix_done_ja_db_);
}

