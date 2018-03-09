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
  phrase_master_db_path_ = path + "/phrase";
  hanzi_master_db_path_ = path + "/hanzi";
  phoneme_master_db_path_ = path + "/phoneme";
  openDB();
}

TTSMaster::~TTSMaster() {
  closeDB();
}


string TTSMaster::getPhoneme(const string &word) {
  string result;
  return result;
}





#define OPEN_DB(x) { \
  if (x#master_ == nullptr) {\
    leveldb::Options options;\
    options.create_if_missing = true;\
    options.max_open_files = 512;\
    options.paranoid_checks = true;\
    options.compression = leveldb::kNoCompression;\
    auto status = leveldb::DB::Open(options, x#master_db_path_, &x#master_);\
    if (status.ok() == false) {\
      DUMP_VAR(status.ToString());\
      x#master_ = nullptr;\
    }\
  }\
}

#define CLOSE_DB(x) { \
  if (x#master_ != nullptr) {\
    delete x#master_;\
    x#master_ = nullptr;\
  }\
}

void TTSMaster::openDB() {
  OPEN_DB(phrase);
  OPEN_DB(hanzi);
  OPEN_DB(phoneme);
}

void TTSMaster::closeDB() {
  CLOSE_DB(phrase);
  CLOSE_DB(hanzi);
  CLOSE_DB(phoneme);
}
