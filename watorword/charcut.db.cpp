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

#include "charcut.hpp"

#define DUMP_VAR(x)                                                          \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">" \
            << std::endl;
#define TRACE_VAR(x)

static leveldb::DB *gSaveDB = nullptr;

void CharCut::openDB() {
  if (gSaveDB == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, "./db/baidu.baike/word_statistics",
                                    &gSaveDB);
    DUMP_VAR(status.ToString());
    if (status.ok() == false) {
      gSaveDB = nullptr;
    }
  }
}

void CharCut::closeDB() {
  if (gSaveDB != nullptr) {
    delete gSaveDB;
    gSaveDB = nullptr;
  }
}
void CharCut::writeDB() {
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

int iConstSnapshotCounter = 100;
void CharCut::dumpSnapshotDB() {
  static int iCounter = 0;
  if (iCounter++ % iConstSnapshotCounter != iConstSnapshotCounter - 1) {
    return;
  }
  static int iSnapshotNumber = 0;
  string pathDump =
      (boost::format("%s.%08d") %
       "./db/baidu.baike/snapshot/word_statistics_iter" % iSnapshotNumber++)
          .str();
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

void CharCut::pushMultiWordGlobal(const string &word, int counter) {
  leveldb::ReadOptions readOptions;
  readOptions.verify_checksums = true;
  string valueStr;
  leveldb::Slice key(word);
  if (gSaveDB != nullptr) {
    auto status = gSaveDB->Get(readOptions, key, &valueStr);
    DUMP_VAR(status.ToString());
    DUMP_VAR(valueStr);
    int sum = counter;
    if (status.ok()) {
      sum += std::stoi(valueStr);
    }
    DUMP_VAR(sum);
    leveldb::Slice value(std::to_string(sum));
    gSaveDBBatch.Put(key, value);
  }
}
