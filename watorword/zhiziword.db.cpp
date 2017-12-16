#include <algorithm>
#include <cmath>
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
#include "zhiziword.hpp"


map<string, int> ZhiZiWord::multiWordOfOneArticle_;

bool ZhiZiWord::loadMaster(bool forceCast) {
  if(dictInputCN_.loadMasterFromDB(database_word_+"/cn") == false) {
    return false;
  }
  if(dictInputJA_.loadMasterFromDB(database_word_+"/ja") == false) {
    return false;
  }
  return true;
}

void ZhiZiWord::unloadMaster(void) {
  dictInputCN_.unloadMasterFromDB();
  dictInputJA_.unloadMasterFromDB();
}



void ZhiZiWord::collectWord(void) {
  for(auto wordPair:wordSeqTopSelected_) {
    auto word = std::get<0>(wordPair.second);
    DUMP_VAR(word);
    auto it = multiWordOfOneArticle_.find(word);
    if (it != multiWordOfOneArticle_.end()) {
      it->second++;
    } else {
      multiWordOfOneArticle_[word] = 1;
    }
  }
}


void ZhiZiWord::dumpPreds() {
  for (auto elem : wordSeqTopSelected_) {
    TRACE_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word, pos, range, weight);
  }
}

void ZhiZiWord::dumpClearSeq() {
  for (auto clearSeq : noConflictWordSeq_) {
    std::cout << "%%%%%%%%%%%%%%%%" << std::endl;
    for (auto elem : clearSeq) {
      auto word = std::get<0>(elem.second);
      auto pos = std::get<1>(elem.second);
      auto range = std::get<2>(elem.second);
      auto weight = std::get<3>(elem.second);
      auto weight_orig = std::get<4>(elem.second);
      DUMP_VAR5(word, pos, range, weight, weight_orig);
    }
    std::cout << "%%%%%%%%%%%%%%%%" << std::endl;
  }
}


/*
void ZhiZiWord::dumpPredWords() {
  for (auto phoenix : prediWords_) {
  std:
    cout << "'" << phoenix << "',";
  }
  std::cout << std::endl;
}
*/



#include <boost/graph/directed_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <sstream>
#include <string>
#include <utility>
using namespace boost;
using namespace boost::graph_detail;

#include <boost/algorithm/string.hpp>

typedef boost::directed_graph<> Graph;

typedef std::pair<string, string> Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

void ZhiZiWord::dumpDot(void) {
  Graph g;
  multimap<int, std::tuple<string, Vertex>> vertexs;
  static vector<string> labelVertex;
  
  auto vrtxStart = g.add_vertex();
  auto vrtxPrStart = std::make_tuple("S", vrtxStart);
  vertexs.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back("S");
  
  int posLast = 0;
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto vrtx = g.add_vertex();
    auto vrtxPr = std::make_tuple(word, vrtx);
    vertexs.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(word);
    posLast = position + word.size();
  }
  auto vrtxEnd = g.add_vertex();
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd);
  vertexs.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back("E");
 
  // add dummy start.
  {
    auto rangeSelf = vertexs.equal_range(0);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      g.add_edge(vrtxStart, vrtxSelf);
    }
  }
  
  

  
  
  
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto next = position + range;
    auto rangeSelf = vertexs.equal_range(position);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      if (word == wordSelf) {
        auto rangeNext = vertexs.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second;
             itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          g.add_edge(vrtxSelf, vrtxNext);
        }
        break;
      }
    }
  // add dummy end.
    if(posLast == position + range) {
      auto rangeSelf = vertexs.equal_range(position);
      for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
        auto wordSelf = std::get<0>(itSelf->second);
        auto vrtxSelf = std::get<1>(itSelf->second);
        if (word == wordSelf) {
          g.add_edge(vrtxSelf, vrtxEnd);
          break;
        }
      }
    }
  }
  static int counter = 0;
  struct sample_graph_writer {
    void operator()(std::ostream& out, void* data) const {
      auto word = labelVertex.at(counter);
      out << " [ label = \"";
      out << word;
      out << "\" ]";
      counter++;
    }
  };
  sample_graph_writer gw;

  std::stringstream ss;
  boost::write_graphviz(ss, g, gw);
  auto dotStr = ss.str();
  boost::algorithm::replace_all(
      dotStr, "digraph G {",
      "digraph G { \n rankdir=LR;\n graph [charset=\"UTF-8\"];\n");
  DUMP_VAR(dotStr);
  // dot -v -T svg 1.dot -o 1.svg

  counter = 0;
  labelVertex.clear();
}

