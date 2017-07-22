#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "phoenixword.hpp"


#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define DUMP_VAR2(x,y) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "=<" << x << "," << y << ">" << std::endl;
#define DUMP_VAR3(x,y,z) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "=<" << x << "," << y << "," << z <<">" << std::endl;
#define DUMP_VAR4(x,y,z,a) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "=<" << x << "," << y << "," << z << "," << a <<">" << std::endl;
#define DUMP_VAR5(x,y,z,a,b) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "," << #b << "=<" << x << "," << y << "," << z << "," << a << "," << b <<">" << std::endl;

#define TRACE_VAR(...)

bool loadMasterFromDB(const string &path,bool castForce);

bool PhoenixWord::loadMaster(bool castForce) {
 auto master = database_ + "/master/word_parrot";
 DUMP_VAR(master);
 return loadMasterFromDB(master,castForce);
}

void unloadMasterFromDB(void);
void PhoenixWord::unloadMaster(void) {
 unloadMasterFromDB();
}

void PhoenixWord::push2DB(void) {
  for(auto wp:gMultiWordSum){
    TRACE_VAR(wp.first,wp.second);
    if(wp.second >2) {
      dict_.putWord(wp.first,wp.second);
    }
  }
  dict_.writeDB();
}

void PhoenixWord::dumpSeq() {
  for(auto elem :wordSeq_){
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    auto weight_orig = std::get<4>(elem.second);
    DUMP_VAR5(word,pos,range,weight,weight_orig);
  }
}

void PhoenixWord::dumpPreds() {
  for(auto elem :wordSeqTopSelected_){
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word,pos,range,weight);
  }
}

void PhoenixWord::dumpClearSeq() {
  for(auto clearSeq:noConflictWordSeq_) {
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    for(auto elem :clearSeq){
     auto word = std::get<0>(elem.second);
     auto pos = std::get<1>(elem.second);
     auto range = std::get<2>(elem.second);
     auto weight = std::get<3>(elem.second);
     auto weight_orig = std::get<4>(elem.second);
     DUMP_VAR5(word,pos,range,weight,weight_orig);
    }
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
}

void PhoenixWord::dumpRank() {
  for(auto rPair:phoenixRank_){
    auto word = rPair.first;
    auto weight = std::get<0>(rPair.second);
    auto weight_orig = std::get<1>(rPair.second);
    DUMP_VAR4(word,word.size(),weight,weight_orig);
  }
}

void PhoenixWord::dumpPredWords() {
  for(auto phoenix:prediWords_) {
    std:cout << "'" << phoenix << "',";
  }
  std::cout << std::endl;
}

