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


TTSMaster::TTSMaster(const string &path) {
  master_db_path_ = path ;
}

TTSMaster::~TTSMaster() {}

void TTSMaster::openDB() {
  if (master_ == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, out_db_path_, &master_);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      master_ = nullptr;
    }
  }
}

void TTSMaster::closeDB() {
  if (master_ != nullptr) {
    delete master_;
    master_ = nullptr;
  }
}
string TTSMaster::getPhoneme(const string &word) {
  string result;
  return result;
}

