#include <algorithm>
#include <cfloat>
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
#include "phoenixword.hpp"



static const int gWeightAdjustBase = 4;
static const double gWeightAdjustRateMax = 16.0;

PhoenixWord::PhoenixWord(const string &database) : database_(database) {}
PhoenixWord::~PhoenixWord() {}
void PhoenixWord::learn(const vector<string> &wordBytes, const string &text,const string &lang) {
  if (wordBytes.size() < 2) {
    return;
  }
  TRACE_VAR(text);
  for (auto mbyte : wordBytes) {
    TRACE_VAR(mbyte);
  }
  this->getRawRank(wordBytes,lang);
  this->dumpDot();
  //this->adjustRank();
  //  this->dumpRank();

  //this->cutTextByRank(text);
  //  this->dumpSeq();

  this->getNoConflictSeq();
  this->dumpClearSeq();

  this->calcPrediction();
}


void PhoenixWord::commitArticle(void) {
  multiWordOfOneArticle_.clear();
}

void PhoenixWord::getRawRank(const vector<string> &Bytes,const string &lang) {
  statisticsMinWordSize_ = 32;
  wordHintSeq_.clear();
  statisticsRank_.clear();
  noConflictWordSeq_.clear();
  wordSeqTopSelected_.clear();
  ap_++;

  list<string> preWords;
  TRACE_VAR(statisticsMinWordSize_);
  int baseLinePos = 0;
  for (auto &word : Bytes) {
    TRACE_VAR(word);
    int wordPos = baseLinePos;
    /* single */
    {
      auto pred = -1.0;
      if(lang =="cn") {
        pred = dictInputCN_.getDoublePred(word);
      }
      if(lang =="ja") {
        pred = dictInputJA_.getDoublePred(word);
      }
      if (pred > 0) {
        TRACE_VAR(word, pred);
        TRACE_VAR(word.size());
        if (statisticsMinWordSize_ > word.size()) {
          statisticsMinWordSize_ = word.size();
        }
        statisticsRank_[word] = std::make_tuple(pred, pred);
        TRACE_VAR(word, wordPos, pred);
        auto elem =
            std::make_tuple(word, wordPos, word.size(), pred, pred);
        wordHintSeq_.insert(std::make_pair(wordPos, elem));
      }      
    }
    /* multi */
    auto it = preWords.rbegin();
    string jointWord(word);
    while (it != preWords.rend()) {
      jointWord = *it + jointWord;
      TRACE_VAR(jointWord);
      auto pred = -1.0;
      if(lang =="cn") {
        pred = dictInputCN_.getDoublePred(jointWord);
      }
      if(lang =="ja") {
        pred = dictInputJA_.getDoublePred(jointWord);
      }
      wordPos -= it->size();
      if (pred > 0) {
        TRACE_VAR(jointWord, pred);
        TRACE_VAR(jointWord.size());
        if (statisticsMinWordSize_ > jointWord.size()) {
          statisticsMinWordSize_ = jointWord.size();
        }
        statisticsRank_[jointWord] = std::make_tuple(pred, pred);

        TRACE_VAR(jointWord, wordPos, pred);
        auto elem =
            std::make_tuple(jointWord, wordPos, jointWord.size(), pred, pred);
        wordHintSeq_.insert(std::make_pair(wordPos, elem));
      }
      it++;
    }
    baseLinePos += word.size();
    preWords.push_back(word);
    if (preWords.size() > gWordLength - 1) {
      preWords.pop_front();
    }
  }
  TRACE_VAR(statisticsMinWordSize_);
}



void PhoenixWord::adjustRank() {
  for (auto rPair : statisticsRank_) {
    TRACE_VAR(rPair.first, rPair.second);
    auto word = rPair.first;
    auto weight = std::get<0>(rPair.second);
    TRACE_VAR(word, weight, word.size());
    double rate = (double)word.size() / (double)statisticsMinWordSize_ - 1.0;
    double rate2 = ::pow(gWeightAdjustBase, rate);
    if (rate > gWeightAdjustRateMax) {
      DUMP_VAR4(word, word.size(), statisticsMinWordSize_, rate);
      rate2 = 1.0;
    }
    TRACE_VAR(rate, rate2);
    auto weight_adj = weight * (int)rate2;
    TRACE_VAR(word, word.size(), weight, weight_adj);
    statisticsRank_[word] = std::make_tuple(weight_adj, weight);
    ;
  }
}



#include <boost/algorithm/string.hpp>

#if 0
void PhoenixWord::getWordPrediction(const string &text) {
  string textRemain(text);
  for (auto elem : wordSeqTopSelected_) {
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word, pos, range, weight);
    boost::algorithm::replace_all(textRemain, word, " ");
    prediWords_.push_back(word);
  }
  DUMP_VAR(textRemain);
  list<string> list_textRemain;
  boost::split(list_textRemain, textRemain, boost::is_space(),
               boost::algorithm::token_compress_on);
  for (auto remain : list_textRemain) {
    TRACE_VAR(remain);
    TRACE_VAR(remain.size());
    if (remain.size() > 1 && remain.size() < 4) {
      prediWords_.push_back(remain);
    }
  }
}
#endif

void PhoenixWord::getNoConflictSeq(void) {
  multimap<int, WordElement> nocfWordSeq;
  int maxPreCover = 0;
  for (auto it = wordHintSeq_.begin(); it != wordHintSeq_.end(); it++) {
    if (wordHintSeq_.begin() == it) {
      nocfWordSeq.insert(std::make_pair(it->first, it->second));
      continue;
    }
    auto prevIt = std::prev(it);
    auto wordPre = std::get<0>(prevIt->second);
    auto posPre = std::get<1>(prevIt->second);
    auto rangePre = std::get<2>(prevIt->second);
    auto preCover = posPre + rangePre;
    if (maxPreCover < preCover) {
      maxPreCover = preCover;
    }
    auto curWord = std::get<0>(it->second);
    auto curPos = std::get<1>(it->second);
    if (maxPreCover <= curPos) {
      TRACE_VAR(curWord, curPos, true);
      noConflictWordSeq_.push_back(nocfWordSeq);
      nocfWordSeq.clear();
      maxPreCover = 0;
    } else {
      TRACE_VAR(curWord, curPos, false);
    }
    nocfWordSeq.insert(std::make_pair(it->first, it->second));
  }
  TRACE_VAR(noConflictWordSeq_.size());
  if (nocfWordSeq.empty() == false) {
    noConflictWordSeq_.push_back(nocfWordSeq);
  }
}

void PhoenixWord::calcPrediction(void) {
  for (auto clearSeq : noConflictWordSeq_) {
    // std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    /*
        for(auto elem :clearSeq){
         auto word = std::get<0>(elem.second);
         auto pos = std::get<1>(elem.second);
         auto range = std::get<2>(elem.second);
         auto weight = std::get<3>(elem.second);
         auto weight_orig = std::get<4>(elem.second);
         DUMP_VAR5(word,pos,range,weight,weight_orig);
        }
    */
    this->calcPrediction(clearSeq);
    // std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
}



#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include <iostream>

typedef boost::adjacency_list<> Graph;
typedef Graph graph_t;
typedef Graph::vertex_descriptor Vertex;


using namespace boost;

typedef graph_traits < graph_t >::vertices_size_type Size;

typedef  iterator_property_map<std::vector<Size>::iterator,
                        property_map<graph_t, vertex_index_t>::const_type>
    dtime_pm_type;



#if 1
template < typename TimeMap > class bfs_time_visitor:public default_bfs_visitor {
  typedef typename property_traits < TimeMap >::value_type T;
public:
  bfs_time_visitor(TimeMap tmap, T & t):m_timemap(tmap), m_time(t) { }
  template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph & g) const
  {
    put(m_timemap, u, m_time++);
  }
  TimeMap m_timemap;
  T & m_time;
};


#endif


#if 0
struct my_bfs_visitor : boost::default_bfs_visitor{

    void initialize_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      std::cout << "Initialize: " << g[s] << std::endl;
    }
    void discover_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      std::cout << "Discover: " << g[s] << std::endl;
    }
    void examine_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      std::cout << "Examine vertex: " << g[s] << std::endl;
    }
    void examine_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      std::cout << "Examine edge: " << g[e] << std::endl;
    }
    void tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      std::cout << "Tree edge: " << g[e] << std::endl;
    }
    void non_tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      std::cout << "Non-Tree edge: " << g[e] << std::endl;
    }
    void gray_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
      std::cout << "Gray target: " << g[e] << std::endl;
    }
    void black_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
      std::cout << "Black target: " << g[e] << std::endl;
    }
    void finish_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      std::cout << "Finish vertex: " << g[s] << std::endl;
    }
  };
#endif



void PhoenixWord::calcPrediction(const multimap<int, WordElement> &confuse) {
  Graph g;
  multimap<int, std::tuple<string, Vertex>> vertexs;
  static vector<string> labelVertex;
  
  auto vrtxStart = add_vertex(g);
  auto vrtxPrStart = std::make_tuple("S", vrtxStart);
  vertexs.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back("S");
  
  int posLast = 0;
  for (auto elem : confuse) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto vrtx = add_vertex(g);
    auto vrtxPr = std::make_tuple(word, vrtx);
    vertexs.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(word);
    posLast = position + word.size();
  }
  auto vrtxEnd = add_vertex(g);
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd);
  vertexs.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back("E");
 
  // add dummy start.
  {
    auto rangeSelf = vertexs.equal_range(0);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      add_edge(vrtxStart, vrtxSelf,g);
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
          add_edge(vrtxSelf, vrtxNext,g);
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
          add_edge(vrtxSelf,vrtxEnd,g);
          break;
        }
      }
    }
  }

  std::vector < Size > dtime(num_vertices(g));
  dtime_pm_type dtime_pm(dtime.begin(), get(vertex_index, g));
  Size time = 0;
  bfs_time_visitor < dtime_pm_type >vis(dtime_pm, time);

  boost::breadth_first_search(g, boost::vertex(vrtxStart, g), boost::visitor(vis));
  
  std::vector<graph_traits<graph_t>::vertices_size_type > discover_order(vrtxEnd);

  std::sort(discover_order.begin(), discover_order.end(),
            indirect_cmp < dtime_pm_type, std::less < Size > >(dtime_pm));

  std::cout << "order of discovery: ";
  for (int i = 0; i < labelVertex.size(); ++i) {
    //DUMP_VAR2(dotStr);
    std::cout << "i:"<< discover_order[i] << ":" << labelVertex.at(discover_order[i]) << " ";
  }
  std::cout << std::endl;
  
  
  /*
  std::vector<Vertex> parents(boost::num_vertices(g));
  boost::dijkstra_shortest_paths(g, vrtxStart,
                boost::predecessor_map(&parents[0]));
  */
  
  struct sample_graph_writer {
    void operator()(std::ostream& out, int i) const {
      auto word = labelVertex.at(i);
      out << " [ label = \"";
      out << word;
      out << "\" ]";
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

  labelVertex.clear();
}

