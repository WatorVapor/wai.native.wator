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

#include "urlstorage.hpp"
#include "log.hpp"



URLStorage::URLStorage (const string &path) {
  out_db_path_ = path + "/";
  DUMP_VAR(out_db_path_);
  iter_db_path_ = path + "/snapshot/" + "iter.";
  DUMP_VAR(iter_db_path_);
}

URLStorage::~URLStorage() {
}

void URLStorage::openDB() {
  if (save_ == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, out_db_path_, &save_);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      save_ = nullptr;
    }
  }
}

void URLStorage::closeDB() {
  if (save_ != nullptr) {
    delete save_;
    save_ = nullptr;
  }
}
void URLStorage::writeDB() {
  if (save_ != nullptr) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    auto status = save_->Write(writeOptions, &saveBatch_);
    DUMP_VAR(status.ToString());
    if (status.ok()) {
      saveBatch_.Clear();
      dumpSnapshotDB();
    }
  }
}

void URLStorage::gets(int max,vector<std::string> &urls) {
  if (save_) {
    leveldb::ReadOptions readOptions;
    readOptions.snapshot = save_->GetSnapshot();
    auto it = save_->NewIterator(readOptions);
    it->SeekToFirst();
    DUMP_VAR(it->Valid());
    int number = 0;
    while (it->Valid()) {
      if(++number > max){
        break;
      }
      urls.push_back(it->value().ToString());
      it->Next();
    }
    delete it;
    save_->ReleaseSnapshot(readOptions.snapshot);
  }
}

void URLStorage::add(const std::string &key,const std::string &value) {
  if (save_) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    leveldb::Slice sKey(key);
    leveldb::Slice sValue(value);
    auto status = save_->Put(writeOptions,sKey,sValue);
    DUMP_VAR(status.ToString());
  }
}
void URLStorage::remove(const std::string &key) {
  if (save_) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    std::string value;
    leveldb::Slice sKey(key);
    auto status = save_->Delete(writeOptions,sKey);
    DUMP_VAR(status.ToString());
  }
}
bool URLStorage::is_has(const std::string &key) {
  if (save_) {
    leveldb::ReadOptions readOptions;
    std::string value;
    leveldb::Slice sKey(key);
    auto status = save_->Get(readOptions,sKey,&value);
    DUMP_VAR(status.ToString());
    if(status.ok()) {
      return true;
    }
  }
  return false;
}
string URLStorage::summary(void) {
  string sum;
  return sum;
}



static int iConstSnapshotCounter = 100;
void URLStorage::dumpSnapshotDB() {
  static int iCounter = 0;
  if (iCounter++ % iConstSnapshotCounter != iConstSnapshotCounter - 1) {
    return;
  }
  static int iSnapshotNumber = 0;
  string pathIter = (boost::format("%08d") % iSnapshotNumber++).str();
  string pathDump = iter_db_path_ + pathIter;
  leveldb::DB *dumpdb = nullptr;
  leveldb::Options options;
  options.create_if_missing = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(options, pathDump, &dumpdb);
  if (status.ok()) {
    leveldb::WriteBatch batch;
    if (save_) {
      leveldb::ReadOptions readOptions;
      readOptions.snapshot = save_->GetSnapshot();
      auto it = save_->NewIterator(readOptions);
      it->SeekToFirst();
      DUMP_VAR(it->Valid());
      while (it->Valid()) {
        batch.Put(it->key(), it->value());
        it->Next();
      }
      delete it;
      save_->ReleaseSnapshot(readOptions.snapshot);
    }
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    auto status = dumpdb->Write(writeOptions, &batch);
    delete dumpdb;
  }
}


