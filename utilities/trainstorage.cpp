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


static leveldb::DB *gSaveDB = nullptr;

TrainTaskStorage::TrainTaskStorage(const string &path, const string &prefix) {
  out_db_path_ = path + "/" + prefix;
  DUMP_VAR(out_db_path_);
}

TrainTaskStorage::~TrainTaskStorage() {}

void TrainTaskStorage::open(void) {
  if (gSaveDB == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, out_db_path_, &gSaveDB);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      gSaveDB = nullptr;
    }
  }
}

void TrainTaskStorage::close(void) {
  if (gSaveDB != nullptr) {
    delete gSaveDB;
    gSaveDB = nullptr;
  }
}

