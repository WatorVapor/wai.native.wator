#include <fstream>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

#include "log.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

static leveldb::DB *gSaveDB = nullptr;
static leveldb::WriteBatch gSaveDBBatch;

static void openDB(const string &path) {
  if (gSaveDB == nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 512;
    options.paranoid_checks = true;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, path, &gSaveDB);
    if (status.ok() == false) {
      DUMP_VAR(status.ToString());
      gSaveDB = nullptr;
    }
  }
}

static void closeDB(void) {
  if (gSaveDB != nullptr) {
    delete gSaveDB;
    gSaveDB = nullptr;
  }
}

static void writeDB(void) {
  if (gSaveDB != nullptr) {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;
    auto status = gSaveDB->Write(writeOptions, &gSaveDBBatch);
    DUMP_VAR(status.ToString());
    if (status.ok()) {
      gSaveDBBatch.Clear();
    }
  }
}

int main(int ac, char *av[]) {
  if (ac < 3) {
    DUMP_VAR(ac);
    return 0;
  }
  openDB(av[2]);
  try {
    fs::path path(av[1]);
    int iCounter = 1;
    BOOST_FOREACH (const fs::path &p,
                   std::make_pair(fs::recursive_directory_iterator(path),
                                  fs::recursive_directory_iterator())) {
      if (!fs::is_directory(p)) {
        string pathText = p.string();
        DUMP_VAR(pathText);
        std::ifstream textStream(pathText);
        std::string str((std::istreambuf_iterator<char>(textStream)),
                        std::istreambuf_iterator<char>());
        textStream.close();
        auto filename = p.filename().string();
        DUMP_VAR(filename);
        leveldb::Slice key(filename);
        leveldb::Slice value(str);
        gSaveDBBatch.Put(key, value);
        if (iCounter++ % 100 == 0) {
          writeDB();
        }
      }
    }
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  writeDB();
  closeDB();
  return 0;
}
