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

#include "ttsmaster.hpp"
#include "log.hpp"


DictionaryMaster::DictionaryMaster(const string &path) {
  master_db_path_ = path ;
  master_cast_db_path_ = path + ".cast";
}

DictionaryMaster::~DictionaryMaster() {}

void DictionaryMaster::openDB() {
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

void DictionaryMaster::closeDB() {
  if (gSaveDB != nullptr) {
    delete gSaveDB;
    gSaveDB = nullptr;
  }
}


void DictionaryMaster::putWord(const string &word, int counter) {
  leveldb::ReadOptions readOptions;
  readOptions.verify_checksums = true;
  string valueStr;
  leveldb::Slice key(word);
  if (gSaveDB != nullptr) {
    auto status = gSaveDB->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    TRACE_VAR(valueStr);
    int sum = counter;
    if (status.ok()) {
      try {
        sum += std::stoi(valueStr);
      } catch (std::exception e) {
        DUMP_VAR2(e.what(), valueStr);
      }
    }
    TRACE_VAR(sum);
    leveldb::Slice value(std::to_string(sum));
    gSaveDBBatch.Put(key, value);
  }
}
double DictionaryMaster::getDoublePred(const string &word) {
  return 1.0;
}
