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

#include "parrotword.hpp"
#include "log.hpp"

bool loadMasterFromDB(const string &path, bool forceCast);

bool ParrotWord::loadMaster(bool forceCast) {
  auto master = database_;
  DUMP_VAR(master);
  return loadMasterFromDB(master, forceCast);
}

void unloadMasterFromDB(void);
void ParrotWord::unloadMaster(void) { unloadMasterFromDB(); }


void ParrotWord::dumpRank() {
  for (auto rPair : statisticsRank_) {
    auto word = rPair.first;
    auto weight = std::get<0>(rPair.second);
    auto weight_orig = std::get<1>(rPair.second);
    DUMP_VAR4(word, word.size(), weight, weight_orig);
  }
}


void ParrotWord::dumpSeq() {
  for (auto elem : wordSeq_) {
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word, pos, range, weight);
  }
}
void ParrotWord::dumpPreds() {
  for (auto elem : wordSeqTopSelected_) {
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word, pos, range, weight);
  }
}

void ParrotWord::dumpDot(void) {
}

