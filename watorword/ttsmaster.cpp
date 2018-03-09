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
  master_db_path_phrase_ = path + "/phrase";
  master_db_path_hanzi_ = path + "/hanzi";
  master_db_path_phoneme_ = path + "/phoneme";
  openDB();
}

TTSMaster::~TTSMaster() {
  closeDB();
}


string TTSMaster::getPhoneme(const string &pinyin) {
  string result;
  if (master_phoneme_) {
    leveldb::ReadOptions readOptions;
    readOptions.verify_checksums = true;
    leveldb::Slice key(pinyin);
    string valueStr;
    auto status = master_phoneme_->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    if (status.ok() && valueStr.empty() == false) {
      TRACE_VAR(valueStr);
      return valueStr;
    }
  }
  return result;
}

string TTSMaster::getHanzi(const string &hanzi){
  string result;
  if (master_hanzi_) {
    leveldb::ReadOptions readOptions;
    readOptions.verify_checksums = true;
    leveldb::Slice key(hanzi);
    string valueStr;
    auto status = master_hanzi_->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    if (status.ok() && valueStr.empty() == false) {
      TRACE_VAR(valueStr);
      return valueStr;
    }
  }
  return result;
}
string TTSMaster::getPhrase(const string &word){
  string result;
  if (master_phrase_) {
    leveldb::ReadOptions readOptions;
    readOptions.verify_checksums = true;
    leveldb::Slice key(word);
    string valueStr;
    auto status = master_phrase_->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    if (status.ok() && valueStr.empty() == false) {
      TRACE_VAR(valueStr);
      return valueStr;
    }
  }
  return result;
}




#define OPEN_DB(x) { \
  if (master_##x##_ == nullptr) {\
    leveldb::Options options;\
    options.create_if_missing = true;\
    options.max_open_files = 512;\
    options.paranoid_checks = true;\
    options.compression = leveldb::kNoCompression;\
    auto status = leveldb::DB::Open(options, master_db_path_##x##_, & master_##x##_);\
    if (status.ok() == false) {\
      DUMP_VAR(status.ToString());\
      master_##x##_ = nullptr;\
    }\
  }\
}

#define CLOSE_DB(x) { \
  if (master_##x##_ != nullptr) {\
    delete master_##x##_;\
    master_##x##_ = nullptr;\
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
