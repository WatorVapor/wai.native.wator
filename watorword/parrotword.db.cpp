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

#include <utility>
#include <string>
#include <sstream>
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
using namespace boost;
using namespace boost::graph_detail;

typedef boost::directed_graph<> Graph;

typedef std::pair<string, string> Edge;
typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
//typedef boost::graph_traits < Graph >::vertex_property_type VertexPt;

void ParrotWord::dumpDot(void) {
  Graph g;
  multimap<int, std::tuple<string,Vertex>> vertexs;
  //char* labelVertex[] = new char* [wordHintSeq_.size()];
  //vector<string> labelVertex;
  int i = 0;
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    //auto vrtx = g.add_vertex(word);
    auto vrtx = g.add_vertex();
    auto vrtxPr = std::make_tuple(word,vrtx);
    vertexs.insert(std::make_pair(position,vrtxPr));
    
    //labelVertex[i] = new char [word.size() +1];
    //strncpy(labelVertex[i],word.c_str(),word.size());
    //labelVertex[i][word.size()] =0;
    i++;
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
      if(word == wordSelf) {
        auto rangeNext = vertexs.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second; itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          g.add_edge(vrtxSelf, vrtxNext);
        }
        break;
      }
    }
  }
  //auto lw = boost::make_label_writer(labelVertex);
  
  struct sample_graph_writer {
    void operator()(std::ostream& out) const {
      out << "graph [bgcolor=lightgrey]" << std::endl;
      out << "node [shape=circle color=white]" << std::endl;
      out << "edge [style=dashed]" << std::endl;
    }
  };
  sample_graph_writer gw;
  
  std::stringstream ss;
  boost::write_graphviz(ss, g,gw);
  DUMP_VAR(ss.str());
  /*
  for(auto p :labelVertex) {
    delete [] p;
  }
  delete [] labelVertex;
  */
}

