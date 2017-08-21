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

#define DUMP_VAR(x)                                                          \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">" \
            << std::endl;
#define TRACE_VAR(x)

static leveldb::DB *gMasterdb = nullptr;
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
    return true;
  }
}

void unloadMasterFromDB(void) {
  if (gMasterdb) {
    delete gMasterdb;
    gMasterdb = nullptr;
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
