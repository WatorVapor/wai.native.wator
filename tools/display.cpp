#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "log.hpp"


leveldb::DB* openLastSnapshot(void);

static string gFilterParam;
int main(int ac,char*av[])
{
  if(ac > 1) {
    gFilterParam = string(av[1]);
  }
  leveldb::DB* db = nullptr;
  db = openLastSnapshot();
  if(db == nullptr){
    DUMP_VAR(db);
    return 0;
  }
  leveldb::ReadOptions readOptions;
  readOptions.snapshot = db->GetSnapshot();
  auto it = db->NewIterator(readOptions);
  it->SeekToFirst();
  DUMP_VAR(it->Valid());
  int total =0;
  std::map<int,vector<string>> ranking;
  while(it->Valid()){
    auto keyStr = it->key().ToString();
    auto valueStr = it->value().ToString();
    TRACE_VAR(keyStr,valueStr);
    try {
      auto value = std::stoi(valueStr);
      TRACE_VAR(value);
      if(value > 15){
        //DUMP_VAR(keyStr);
        //DUMP_VAR(valueStr);
        auto it = ranking.find(value);
        if(it == ranking.end()){
          vector<string> words({keyStr}); 
          ranking[value] = words;
        } else {
          it->second.push_back(keyStr);
        }
      }
    } catch(const std::exception &e) {
      DUMP_VAR3(e.what(),keyStr,valueStr);
    }
    it->Next();
    total++;
  }
  for(auto repeatRank:ranking) {
    DUMP_VAR(repeatRank.first);
    for(auto word:repeatRank.second) {
      std::cout << "'" << word << "',";
    }
    std::cout << std::endl;
  }
  DUMP_VAR(total);
  db->ReleaseSnapshot(readOptions.snapshot);
  delete db;
  return 0;
}

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;

leveldb::DB* openLastSnapshot(void) {
  leveldb::DB* db = nullptr;
  string pathStr("db/baidu.baike/snapshot");
  if(gFilterParam.empty() == false) {
    pathStr = "./";
    pathStr += gFilterParam;
    pathStr += "/snapshot";
  }
  DUMP_VAR(pathStr);
  const fs::path path(pathStr);
  std::map<int,string> snapPathMap;
  BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
    if (fs::is_directory(p)){
      auto extension = p.extension().string();
      //DUMP_VAR(extension);
      auto numStr = boost::algorithm::trim_copy_if (extension, boost::is_any_of(".") );
      //DUMP_VAR(numStr);
      //std::reverse(numStr.begin(), numStr.end());
      auto num = std::stoi(numStr);
      DUMP_VAR(num);
      auto stem = p.stem().string();
      DUMP_VAR(stem);
      if(stem == "word_statistics_iter") {
        auto filename = p.string();
        DUMP_VAR(filename);
        snapPathMap[num] = filename;
      }
      if(stem == "word_ostrich_iter") {
        auto filename = p.string();
        DUMP_VAR(filename);
        snapPathMap[num] = filename;
      }
      if(stem == "word_parrot_iter") {
        auto filename = p.string();
        DUMP_VAR(filename);
        snapPathMap[num] = filename;
      }
      if(stem == "word_phoenix_iter") {
        auto filename = p.string();
        DUMP_VAR(filename);
        snapPathMap[num] = filename;
      }
    }
  }
  for(auto snapPath:snapPathMap) {
    DUMP_VAR(snapPath.first);
    DUMP_VAR(snapPath.second);
  }
  for(auto it = snapPathMap.rbegin();it != snapPathMap.rend();it++){
    DUMP_VAR(it->first);
    DUMP_VAR(it->second);
    leveldb::Options options;
    options.compression = leveldb::kNoCompression;
    auto status = leveldb::DB::Open(options, it->second, &db);
    DUMP_VAR(status.ToString());
    if(status.ok()) {
      return db;
    }
  }
  return db;
}
