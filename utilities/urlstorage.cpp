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


static leveldb::DB *gSaveDB = nullptr;

URLStorage::URLStorage (const string &path) {
  out_db_path_ = path + "/";
  DUMP_VAR(out_db_path_);
  iter_db_path_ = path + "/snapshot/" + "iter.";
  DUMP_VAR(iter_db_path_);
}

URLStorage::~URLStorage() {}

void URLStorage::openDB() {
  if (gSaveDB == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    // auto status = leveldb::DB::Open(options, "./db/baidu.baike/word_ostrich",
    // &gSaveDB);
    auto status = leveldb::DB::Open(options, out_db_path_, &gSaveDB);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      gSaveDB = nullptr;
    }
  }
}

void URLStorage::closeDB() {
  if (gSaveDB != nullptr) {
    delete gSaveDB;
    gSaveDB = nullptr;
  }
}
void URLStorage::writeDB() {
  if (gSaveDB != nullptr) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    auto status = gSaveDB->Write(writeOptions, &gSaveDBBatch);
    DUMP_VAR(status.ToString());
    if (status.ok()) {
      gSaveDBBatch.Clear();
      dumpSnapshotDB();
    }
  }
}

static int iConstSnapshotCounter = 100;
void URLStorage::dumpSnapshotDB() {
  static int iCounter = 0;
  if (iCounter++ % iConstSnapshotCounter != iConstSnapshotCounter - 1) {
    return;
  }
  static int iSnapshotNumber = 0;
  string pathIter = (boost::format("%08d") % iSnapshotNumber++).str();
  // std::reverse(pathIter.begin(), pathIter.end());
  // string pathDump = "./db/baidu.baike/snapshot/word_ostrich_iter." +
  // pathIter;
  string pathDump = iter_db_path_ + pathIter;
  leveldb::DB *dumpdb = nullptr;
  leveldb::Options options;
  options.create_if_missing = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(options, pathDump, &dumpdb);
  if (status.ok()) {
    leveldb::WriteBatch batch;
    if (gSaveDB) {
      leveldb::ReadOptions readOptions;
      readOptions.snapshot = gSaveDB->GetSnapshot();
      auto it = gSaveDB->NewIterator(readOptions);
      it->SeekToFirst();
      DUMP_VAR(it->Valid());
      while (it->Valid()) {
        batch.Put(it->key(), it->value());
        it->Next();
      }
      delete it;
      gSaveDB->ReleaseSnapshot(readOptions.snapshot);
    }
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    auto status = dumpdb->Write(writeOptions, &batch);
    delete dumpdb;
  }
}


