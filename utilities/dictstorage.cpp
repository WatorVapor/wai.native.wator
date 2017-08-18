#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <boost/format.hpp>

#include "dictstorage.hpp"
#include "log.hpp"

DictionaryStorage::DictionaryStorage(const string &path) {
  out_db_path_ = path;
  DUMP_VAR(out_db_path_);
}

DictionaryStorage::~DictionaryStorage() {}

void DictionaryStorage::openDB() {
  if (save_ == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    // auto status = leveldb::DB::Open(options, "./db/baidu.baike/word_ostrich",
    // &gSaveDB);
    auto status = leveldb::DB::Open(options, out_db_path_, &save_);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      save_ = nullptr;
    }
  }
}

void DictionaryStorage::closeDB() {
  if (save_ != nullptr) {
    delete save_;
    save_ = nullptr;
  }
}

void DictionaryStorage::putWord(const string &word, int counter) {
  leveldb::ReadOptions readOptions;
  readOptions.verify_checksums = true;
  string valueStr;
  leveldb::Slice key(word);
  if (save_ != nullptr) {
    auto status = save_->Get(readOptions, key, &valueStr);
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
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    status = save_->Put(writeOptions, key, value);
    DUMP_VAR3(status.ToString(),word,sum);
  }
}
