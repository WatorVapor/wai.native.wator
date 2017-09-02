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
    boost::no_property, boost::property<boost::edge_weight_t, int> > Graph;
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
  multimap<int, std::tuple<string, Vertex>> vertexWator;
  vector<string> labelVertex;
  
  auto vrtxStart = add_vertex(g);
  auto vrtxPrStart = std::make_tuple("S", vrtxStart);
  vertexWator.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back("S");
  
  int posLast = 0;
  for (auto elem : confuse) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto vrtx = boost::add_vertex(g);
    auto vrtxPr = std::make_tuple(word, vrtx);
    vertexWator.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(word);
    posLast = position + word.size();
  }
  auto vrtxEnd = add_vertex(g);
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd);
  vertexWator.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back("E");
 
  // add dummy start.
  {
    auto startPos = confuse.begin()->first;
    auto rangeSelf = vertexWator.equal_range(startPos);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto ed = boost::add_edge(vrtxStart, vrtxSelf,g);
      int weight = boost::get(boost::edge_weight_t(), g, ed.first);
      boost::put(boost::edge_weight_t(), g, ed.first, 1);
      int weight2 = boost::get(boost::edge_weight_t(), g, ed.first);
      DUMP_VAR2(weight,weight2);
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
      if (word == wordSelf) {
        auto rangeNext = vertexWator.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second;
             itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          auto ed = boost::add_edge(vrtxSelf, vrtxNext,g);
          int weight = boost::get(boost::edge_weight_t(), g, ed.first);
          boost::put(boost::edge_weight_t(), g, ed.first, 1);
          int weight2 = boost::get(boost::edge_weight_t(), g, ed.first);
          DUMP_VAR2(weight,weight2);
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
          int weight = boost::get(boost::edge_weight_t(), g, ed.first);
          boost::put(boost::edge_weight_t(), g, ed.first, 1);
          int weight2 = boost::get(boost::edge_weight_t(), g, ed.first);
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
  std::vector<int> distance(num_vertices(g));
  
  auto predmap = boost::predecessor_map(boost::make_iterator_property_map(parents.begin(), boost::get(boost::vertex_index, g))).
                          distance_map(boost::make_iterator_property_map(distance.begin(), boost::get(boost::vertex_index, g)));
  
  boost::dijkstra_shortest_paths(g,vrtxStart,predmap);
  
  
  graph_traits < Graph >::vertex_iterator vi, vend;
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
  
  /*
  for(auto v = vrtxStart; v != vrtxEnd; v = parents[v]) {
    DUMP_VAR(v);
  }
  */
  
}

