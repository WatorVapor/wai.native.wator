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


#if 0
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <iostream>
#endif

//typedef boost::adjacency_list<> Graph;
//typedef boost::adjacency_list<boost::vecS, boost::hash_setS, boost::undirectedS, uint32_t, uint32_t, boost::no_property> Graph;
//typedef Graph graph_t;
//typedef Graph::vertex_descriptor Vertex;


using namespace boost;

#if 0
typedef graph_traits < graph_t >::vertices_size_type Size;

typedef  iterator_property_map<std::vector<Size>::iterator,
                        property_map<graph_t, vertex_index_t>::const_type>
    dtime_pm_type;
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
      //std::cout << "Initialize: " << g[s] << std::endl;
    }
    void discover_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      //std::cout << "Discover: " << g[s] << std::endl;
    }
    void examine_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      //std::cout << "Examine vertex: " << g[s] << std::endl;
    }
    void examine_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      //std::cout << "Examine edge: " << g[e] << std::endl;
    }
    void tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      //std::cout << "Tree edge: " << g[e] << std::endl;
    }
    void non_tree_edge(const graph_t::edge_descriptor &e, const graph_t &g) const {
      //std::cout << "Non-Tree edge: " << g[e] << std::endl;
    }
    void gray_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
      //std::cout << "Gray target: " << g[e] << std::endl;
    }
    void black_target(const graph_t::edge_descriptor &e, const graph_t &g) const {
      //std::cout << "Black target: " << g[e] << std::endl;
    }
    void finish_vertex(const graph_t::vertex_descriptor &s, const graph_t &g) const {
      //std::cout << "Finish vertex: " << g[s] << std::endl;
    }
  };
#endif


#if 0

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
      auto ed = boost::add_edge(vrtxStart, vrtxSelf,g);
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

  
  
  /*
  std::vector<Vertex> parents(boost::num_vertices(g));
  boost::dijkstra_shortest_paths(g, vrtxStart,
                boost::predecessor_map(&parents[0]));
  */
#if 0  
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
#endif
  
#if 0 
  std::vector < Size > dtime(num_vertices(g));
  dtime_pm_type dtime_pm(dtime.begin(), get(vertex_index, g));
  Size time = 0;
  bfs_time_visitor < dtime_pm_type >vis(dtime_pm, time);

  boost::breadth_first_search(g,vrtxStart,boost::visitor(vis));
  
  std::vector<graph_traits<graph_t>::vertices_size_type > discover_order(vrtxEnd);
  integer_range < int >range(0, vrtxEnd);
  std::copy(range.begin(), range.end(), discover_order.begin());
  std::sort(discover_order.begin(), discover_order.end(),
            indirect_cmp < dtime_pm_type, std::less < Size > >(dtime_pm));

  DUMP_VAR(labelVertex.size());
  std::cout << "order of discovery: ";
  for (int i = 0; i < labelVertex.size(); ++i) {
    std::cout << i << ":"<< discover_order[i] << ":" << labelVertex.at(discover_order[i]) << " ";
  }
  std::cout << std::endl;
#endif
 my_bfs_visitor vis;
 boost::breadth_first_search(g, vrtxStart, boost::visitor(vis).vertex_index_map(get(boost::vertex_bundle,g)));
#if 0  
  std::vector<Vertex> parents(boost::num_vertices(g));
  boost::dijkstra_shortest_paths(g, vrtxStart,
                boost::predecessor_map(&parents[0]));
  
  for (Vertex v = vrtxStart; v != vrtxEnd; v = parents[v]) {
  }
#endif
  
  labelVertex.clear();
}
#endif


#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>


typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
    boost::no_property, boost::property<boost::edge_weight_t, int> > Graph;
typedef std::pair<int, int>                             Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor   Vertex;


void PhoenixWord::calcPrediction(const multimap<int, WordElement> &confuse) {
  Graph g;
  multimap<int, std::tuple<string, Vertex>> vertexWator;
  static vector<string> labelVertex;
  
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
  
  
  
  DUMP_VAR2(boost::num_vertices(g),vrtxStart,vrtxEnd);
  std::vector<Vertex> parents(boost::num_vertices(g));
  std::vector<std::size_t> distance(boost::num_vertices(g));
  boost::dijkstra_shortest_paths(g, vrtxStart,
              boost::predecessor_map(&parents[0]).distance_map(&distance[0]));
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

