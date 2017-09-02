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

#include "log.hpp"
#include "phoenixword.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>


typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
    boost::no_property, boost::property<boost::edge_weight_t, float> > Graph;
typedef std::pair<int, int>                             Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor   Vertex;


struct sample_graph_writer {
  void operator()(std::ostream& out, int i) const {
    auto word = labelVertex_.at(i);
    out << " [ label = \"";
    out << word;
    out << "\" ]";
  }
  sample_graph_writer(vector<string> &labelVertex):labelVertex_(labelVertex) {
  }
  vector<string> &labelVertex_;
};


void PhoenixWord::calcPrediction(const multimap<int, WordElement> &confuse) {
  Graph g;
  multimap<int, std::tuple<string, Vertex,double,double>> vertexWator;
  vector<string> labelVertex;
  
  auto vrtxStart = boost::add_vertex(g);
  auto vrtxPrStart = std::make_tuple("S", vrtxStart,1.0,1.0);
  vertexWator.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back("S");
  
  int posLast = 0;
  for (auto elem : confuse) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto weight = std::get<3>(elem.second);
    auto weightO = std::get<4>(elem.second);
    auto vrtx = boost::add_vertex(g);
    auto vrtxPr = std::make_tuple(word, vrtx,weight,weightO);
    vertexWator.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(word);
    posLast = position + word.size();
  }
  auto vrtxEnd = add_vertex(g);
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd,1.0,1.0);
  vertexWator.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back("E");
 
  // add dummy start.
  {
    auto startPos = confuse.begin()->first;
    auto rangeSelf = vertexWator.equal_range(startPos);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto weightR = std::get<2>(itSelf->second);
      auto weightRO = std::get<3>(itSelf->second);
      auto ed = boost::add_edge(vrtxStart, vrtxSelf,g);
      auto weightOld = boost::get(boost::edge_weight_t(), g, ed.first);
      boost::put(boost::edge_weight_t(), g, ed.first, 1.0/weightR);
      auto weightNew = boost::get(boost::edge_weight_t(), g, ed.first);
      DUMP_VAR4(weightR,weightRO,weightOld,weightNew);
    }
  }
  

  
  
  
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto next = position + range;
    auto rangeSelf = vertexWator.equal_range(position);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto weight = std::get<2>(itSelf->second);
      if (word == wordSelf) {
        auto rangeNext = vertexWator.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second;
             itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          auto ed = boost::add_edge(vrtxSelf, vrtxNext,g);
          auto weightOld = boost::get(boost::edge_weight_t(), g, ed.first);
          boost::put(boost::edge_weight_t(), g, ed.first, 1.0/weight);
          auto weightNew = boost::get(boost::edge_weight_t(), g, ed.first);
          DUMP_VAR2(weightOld,weightNew);
        }
        break;
      }
    }
    // add dummy end.
    if(posLast == position + range) {
      auto rangeSelf = vertexWator.equal_range(position);
      for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
        auto wordSelf = std::get<0>(itSelf->second);
        auto vrtxSelf = std::get<1>(itSelf->second);
        if (word == wordSelf) {
          auto ed = boost::add_edge(vrtxSelf,vrtxEnd,g);
          auto weight = boost::get(boost::edge_weight_t(), g, ed.first);
          boost::put(boost::edge_weight_t(), g, ed.first, 0.0);
          auto weight2 = boost::get(boost::edge_weight_t(), g, ed.first);
          DUMP_VAR2(weight,weight2);
          break;
        }
      }
    }
  }
  
  sample_graph_writer gw(labelVertex);

  std::stringstream ss;
  boost::write_graphviz(ss, g, gw);
  auto dotStr = ss.str();
  boost::algorithm::replace_all(
      dotStr, "digraph G {",
      "digraph G { \n rankdir=LR;\n graph [charset=\"UTF-8\"];\n");
  DUMP_VAR(dotStr);
  // dot -v -T svg 1.dot -o 1.svg
  
  
  
  DUMP_VAR3(boost::num_vertices(g),vrtxStart,vrtxEnd);
  
  std::vector<Vertex> parents(boost::num_vertices(g));
  std::vector<float> distance(boost::num_vertices(g));
  
  auto predmap = boost::predecessor_map(boost::make_iterator_property_map(parents.begin(), boost::get(boost::vertex_index, g))).
                          distance_map(boost::make_iterator_property_map(distance.begin(), boost::get(boost::vertex_index, g)));
  
  boost::dijkstra_shortest_paths(g,vrtxStart,predmap);
  
  
  boost::graph_traits < Graph >::vertex_iterator vi, vend;
  for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    std::cout << "distance(" << labelVertex.at(*vi) << ") = " << distance[*vi] << ", ";
    std::cout << "parent(" << labelVertex.at(*vi) << ") = " << labelVertex.at(parents[*vi]) << std::endl;
  }
  
  DUMP_VAR(distance.at(vrtxEnd));

  if (parents[vrtxEnd] == vrtxEnd) {
    std::cout << "no path" << std::endl;
    return ;
  }
  for(auto v:parents) {
    DUMP_VAR(v);
  }
  std::vector<int> path;
  for(auto v = vrtxEnd; v != vrtxStart; v = parents[v]) {
    path.push_back(v);
  }
  for(auto it = path.rbegin();it != path.rend();it++) {
      auto word = labelVertex.at(*it);
      DUMP_VAR2(*it,word);
  }
}

