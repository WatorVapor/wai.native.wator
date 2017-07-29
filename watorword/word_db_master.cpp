#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "log.hpp"
#include "word_db_master.hpp"

static leveldb::DB *gMasterdb = nullptr;
static leveldb::DB *gMasterDBCast = nullptr;

void dumpMaster();
void loadMasterFromDB(void) {
  leveldb::Options options;
  options.max_open_files = 512;
  options.paranoid_checks = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(
      options, "db/baidu.baike/master/word_statistics", &gMasterdb);
  DUMP_VAR(status.ToString());
  if (status.ok() == false) {
    gMasterdb = nullptr;
  } else {
    // dumpMaster();
  }
}

void castMaster(void);
static bool castMasterDB(const string &path, bool cast = true) {
  leveldb::Options options;
  options.create_if_missing = true;
  options.max_open_files = 512;
  options.paranoid_checks = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(options, path, &gMasterDBCast);
  if (status.ok() == false) {
    DUMP_VAR(status.ToString());
    gMasterDBCast = nullptr;
    return false;
  } else {
    DUMP_VAR(cast);
    if (cast) {
      castMaster();
    }
    return true;
  }
}

static bool isExistsCastDB(const string &path) {
  leveldb::Options options;
  options.create_if_missing = false;
  options.max_open_files = 512;
  options.paranoid_checks = true;
  options.compression = leveldb::kNoCompression;
  static leveldb::DB *testDB;
  auto status = leveldb::DB::Open(options, path, &testDB);
  DUMP_VAR(status.ToString());
  if (status.ok()) {
    delete testDB;
    return true;
  } else {
    return false;
  }
}

bool loadMasterFromDB(const string &path) {
  leveldb::Options options;
  options.max_open_files = 512;
  options.paranoid_checks = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(options, path, &gMasterdb);
  DUMP_VAR(status.ToString());
  if (status.ok() == false) {
    gMasterdb = nullptr;
    return false;
  } else {
    if (castMasterDB(path + ".cast") == false) {
      return false;
    }
    return true;
  }
}

bool loadMasterFromDB(const string &path, bool forceCast) {
  leveldb::Options options;
  options.max_open_files = 512;
  options.paranoid_checks = true;
  options.compression = leveldb::kNoCompression;
  auto status = leveldb::DB::Open(options, path, &gMasterdb);
  DUMP_VAR(status.ToString());
  if (status.ok() == false) {
    gMasterdb = nullptr;
    return false;
  } else {
    if (forceCast) {
      if (castMasterDB(path + ".cast") == false) {
        return false;
      }
    } else {
      if (isExistsCastDB(path + ".cast")) {
        if (castMasterDB(path + ".cast", false) == false) {
          return false;
        }
      } else {
        if (castMasterDB(path + ".cast") == false) {
          return false;
        }
      }
    }
  }
  return true;
}

void unloadMasterFromDB(void) {
  if (gMasterdb) {
    delete gMasterdb;
    gMasterdb = nullptr;
    delete gMasterDBCast;
    gMasterDBCast = nullptr;
  }
}

int getPred(const string &word) {
  if (gMasterdb) {
    leveldb::ReadOptions readOptions;
    readOptions.verify_checksums = true;
    leveldb::Slice key(word);
    string valueStr;
    auto status = gMasterdb->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    if (status.ok() && valueStr.empty() == false) {
      TRACE_VAR(valueStr);
      return std::stoi(valueStr);
    }
  }
  return -1;
}

double getDoublePred(const string &word) {
  if (gMasterdb) {
    leveldb::ReadOptions readOptions;
    readOptions.verify_checksums = true;
    leveldb::Slice key(word);
    string valueStr;
    auto status = gMasterDBCast->Get(readOptions, key, &valueStr);
    TRACE_VAR(status.ToString());
    if (status.ok() && valueStr.empty() == false) {
      TRACE_VAR(valueStr, word);
      return std::stod(valueStr);
    }
  }
  return -1.0;
}

void dumpMaster() {
  if (gMasterdb == nullptr) {
    return;
  }
  leveldb::ReadOptions readOptions;
  readOptions.snapshot = gMasterdb->GetSnapshot();
  auto it = gMasterdb->NewIterator(readOptions);
  it->SeekToFirst();
  DUMP_VAR(it->Valid());
  int total = 0;
  std::map<int, vector<string>> ranking;
  while (it->Valid()) {
    auto keyStr = it->key().ToString();
    auto valueStr = it->value().ToString();
    TRACE_VAR(keyStr);
    TRACE_VAR(valueStr);
    auto value = std::stoi(valueStr);
    TRACE_VAR(value);
    if (value > 16) {
      // DUMP_VAR(keyStr);
      // DUMP_VAR(valueStr);
      auto it = ranking.find(value);
      if (it == ranking.end()) {
        vector<string> words({keyStr});
        ranking[value] = words;
      } else {
        it->second.push_back(keyStr);
      }
    }
    it->Next();
    total++;
  }
  delete it;
  gMasterdb->ReleaseSnapshot(readOptions.snapshot);
  for (auto repeatRank : ranking) {
    DUMP_VAR(repeatRank.first);
    for (auto word : repeatRank.second) {
      std::cout << "'" << word << "',";
    }
    std::cout << std::endl;
  }
  DUMP_VAR(total);
}

void castMaster(void) {
  if (gMasterdb == nullptr) {
    return;
  }
  leveldb::ReadOptions readOptions;
  readOptions.snapshot = gMasterdb->GetSnapshot();
  auto it = gMasterdb->NewIterator(readOptions);
  it->SeekToFirst();
  DUMP_VAR(it->Valid());
  int topRank = 0;
  while (it->Valid()) {
    auto keyStr = it->key().ToString();
    auto valueStr = it->value().ToString();
    TRACE_VAR(keyStr);
    TRACE_VAR(valueStr);
    auto value = std::stoi(valueStr);
    TRACE_VAR(value);
    if (value > topRank) {
      topRank = value;
      // DUMP_VAR(keyStr);
      // DUMP_VAR(valueStr);
    }
    it->Next();
  }
  DUMP_VAR(topRank);

  it->SeekToFirst();
  DUMP_VAR(it->Valid());
  leveldb::WriteOptions writeOptions;
  writeOptions.sync = true;
  while (it->Valid()) {
    auto keyStr = it->key().ToString();
    auto valueStr = it->value().ToString();
    TRACE_VAR(keyStr);
    TRACE_VAR(valueStr);
    auto value = std::stoi(valueStr);
    double castWeight = (double)(value) / (double)(topRank);
    TRACE_VAR(value, castWeight);
    auto status =
        gMasterDBCast->Put(writeOptions, keyStr, std::to_string(castWeight));
    TRACE_VAR(status.ToString());
    if (status.ok()) {
    } else {
      DUMP_VAR(status.ToString());
    }
    it->Next();
  }
  delete it;
  gMasterdb->ReleaseSnapshot(readOptions.snapshot);
}
