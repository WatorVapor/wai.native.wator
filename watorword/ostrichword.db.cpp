#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/format.hpp>

#include "ostrichword.hpp"
#include "log.hpp"

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define TRACE_VAR(x)

void OstrichWord::upWordByArticle2Master(void) {
  DUMP_VAR(multiWordOfOneArticle_.size());
  map<int,vector<string>> localMultiWordRank;
  for(auto wordSum:multiWordOfOneArticle_) {
      TRACE_VAR(wordSum.first,wordSum.second);
      if(wordSum.second > 2) {
          TRACE_VAR(wordSum.first,wordSum.second);
          auto it = localMultiWordRank.find(wordSum.second);
          if(it != localMultiWordRank.end()){
              it->second.push_back(wordSum.first);
          } else {
              localMultiWordRank[wordSum.second] ={wordSum.first};
          }
      }
  }
  DUMP_VAR(localMultiWordRank.size());
    for(auto &record:localMultiWordRank) {
        TRACE_VAR(record.first);
        auto words = record.second;
        for(auto word:words){
            bool isShort= false;
            for(const auto &word2:words){
                auto found = word2.find(word);
                if(found != string::npos && word2 != word){
                    TRACE_VAR(word);
                    TRACE_VAR(word2);
                    isShort= true;
                }
            }
            if(isShort == false) {
                if(stub_) {
                } else {
                 dict_->putWord(word,record.first);
                }
            }
        }
    }
  multiWordOfOneArticle_.clear();
}
