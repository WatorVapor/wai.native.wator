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

#include "log.hpp"
#include "urlstorage.hpp"

URLStorage::URLStorage(const string &path) {
  db_path_ = path;
  DUMP_VAR(db_path_);
}

URLStorage::~URLStorage() {}

void URLStorage::openDB() {
  if (save_ == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, db_path_, &save_);
    DUMP_VAR2(db_path_,status.ToString());
    if (status.ok() == false) {
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

void URLStorage::gets(int max, vector<std::string> &urls) {
  if (save_) {
    leveldb::ReadOptions readOptions;
    readOptions.snapshot = save_->GetSnapshot();
    auto it = save_->NewIterator(readOptions);
    DUMP_VAR(it->Valid());
    it->SeekToFirst();
    DUMP_VAR(it->Valid());
    int number = 0;
    while (it->Valid()) {
      if (++number > max) {
        break;
      }
      urls.push_back(it->value().ToString());
      it->Next();
    }
    delete it;
    save_->ReleaseSnapshot(readOptions.snapshot);
  }
}

void URLStorage::add(const std::string &key, const std::string &value) {
  if (save_) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    leveldb::Slice sKey(key);
    leveldb::Slice sValue(value);
    auto status = save_->Put(writeOptions, sKey, sValue);
    DUMP_VAR(status.ToString());
  }
}
void URLStorage::remove(const std::string &key) {
  if (save_) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    std::string value;
    leveldb::Slice sKey(key);
    auto status = save_->Delete(writeOptions, sKey);
    DUMP_VAR(status.ToString());
  }
}
bool URLStorage::is_has(const std::string &key) {
  if (save_) {
    leveldb::ReadOptions readOptions;
    std::string value;
    leveldb::Slice sKey(key);
    auto status = save_->Get(readOptions, sKey, &value);
    DUMP_VAR(status.ToString());
    if (status.ok()) {
      return true;
    }
  }
  return false;
}
string URLStorage::summary(void) {
  string sum;
  sum += db_path_;
  if (save_) {
    leveldb::ReadOptions readOptions;
    std::string value;
    leveldb::Slice sKey("leveldb.stats");
    auto status = save_->GetProperty(sKey, &value);
    DUMP_VAR(status);
    if (status) {
      DUMP_VAR(sum);
      sum += value;
    }
    readOptions.snapshot = save_->GetSnapshot();
    auto it = save_->NewIterator(readOptions);
    it->SeekToFirst();
    int number = 0;
    while (it->Valid()) {
      ++number;
      it->Next();
    }
    delete it;
    save_->ReleaseSnapshot(readOptions.snapshot);
    sum += "\n";
    sum += "total num =<";
    sum += std::to_string(number);
    sum += ">";
    sum += "\n";
  }
  return sum;
}

void URLStorage::copy(std::shared_ptr<URLStorage> dst) {
  if (this->save_ && dst->save_) {
    leveldb::ReadOptions readOptions;
    readOptions.snapshot = this->save_->GetSnapshot();
    auto it = this->save_->NewIterator(readOptions);
    it->SeekToFirst();
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    while (it->Valid()) {
      DUMP_VAR2(it->key().ToString(), it->value().ToString());
      auto status = dst->save_->Put(writeOptions, it->key(), it->value());
      DUMP_VAR(status.ToString());
      it->Next();
    }
    delete it;
    save_->ReleaseSnapshot(readOptions.snapshot);
  }
}

#include <sstream>
#include  <iomanip>
#include <string>
using namespace std;
#include <openssl/sha.h>

string sha256(const string &msg)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  SHA256_Update(&sha256, msg.c_str(), msg.size());
  SHA256_Final(hash, &sha256);
	stringstream ss;
  for(auto dgt:hash)
  {
    ss << setfill('0') << setw(2) << hex << (int)dgt;
  }
DUMP_VAR(ss.str());
  return ss.str();
}

