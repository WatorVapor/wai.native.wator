#include <iostream>
#include <memory>
#include <string>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class MasterDBWord {
 public:
  MasterDBWord();
  ~MasterDBWord();
  bool loadMasterFromDB(const string &path, bool castForce = false);
  void unloadMasterFromDB(void);
  int getPred(const string &word);
  double getDoublePred(const string &word);
  double getRangeMax(void);
  double getRangeMin(void);

 
 private:
  bool castMasterDB(const string &path, bool cast = true);
  bool isExistsCastDB(const string &path);
  void castMaster(void);
  void dumpMaster(void);
  void findRange(void);

 public:
 private:
  leveldb::DB *gMasterdb = nullptr;
  leveldb::DB *gMasterDBCast = nullptr;
  double dmax_;
  double dmin_;
};
