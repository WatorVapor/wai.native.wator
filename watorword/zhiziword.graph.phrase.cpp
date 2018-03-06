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
#include "zhiziword.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>


typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
    boost::no_property, boost::property<boost::edge_weight_t, float> > Graph;
/*typedef std::pair<int, int>                             Edge;*/
typedef boost::graph_traits<Graph>::vertex_descriptor   Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;


static const double dConstWeightNotFoundFator = 1.0 ;

typedef std::tuple<string,double,double,string,string,string,int> LabelVertex;

struct sample_graph_writer {
  void operator()(std::ostream& out, int i) const {
    auto wordPair = labelVertex_.at(i);
    out << " [ label = <";
    out << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">";
    out << "<TR><TD>";
    out << "<FONT COLOR=\"green\" POINT-SIZE=\"12\">" << std::get<0>(wordPair) << "</FONT>";
    out << "</TD></TR>";
    out << "<TR><TD>";
    out << "<FONT COLOR=\"red\" POINT-SIZE=\"6\">" << 1.0/std::get<1>(wordPair);
    out << " " << std::get<3>(wordPair);
    out << "</FONT>";
    out << "</TD></TR>";
    out << "<TR><TD>";
    out << "<FONT COLOR=\"blue\" POINT-SIZE=\"6\">" << std::get<1>(wordPair);
    out << " " << std::get<4>(wordPair);
    out  << "</FONT>";
    out << "</TD></TR>";
    out << "<TR><TD>";
    out << "<FONT COLOR=\"blue\" POINT-SIZE=\"6\">" << std::get<2>(wordPair);
    out << " " << std::get<5>(wordPair);
    out << "</FONT>";
    out << "</TD></TR>";
    out << "</TABLE>";
    out << ">]";
  }
  sample_graph_writer(vector<LabelVertex> &labelVertex):labelVertex_(labelVertex) {
  }
  vector<LabelVertex> &labelVertex_;
};


struct sample_graph_weight_writer {
  void operator()(std::ostream& out,const Edge& e) const {
    auto weight = boost::get(boost::edge_weight_t(), g_, e);
    out << " [ label = <" << weight << ">]";
  }
  sample_graph_weight_writer(Graph &g):g_(g) {
  }
  Graph &g_;
};


void ZhiZiWord::calcPredictionPhrase(const multimap<int, WordElement> &confuse,const string &lang) {
  Graph g;
  multimap<int, std::tuple<string, Vertex,double,double>> vertexWator;
  vector<LabelVertex> labelVertex;
  
  auto vrtxStart = boost::add_vertex(g);
  auto vrtxPrStart = std::make_tuple("S", vrtxStart,1.0,1.0);
  vertexWator.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back(std::make_tuple("S",1.0,1.0,"","","",-1));
  
  int posLast = 0;
  for (auto elem : confuse) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto weight = std::get<3>(elem.second);
    auto weightO = std::get<4>(elem.second);
    auto vrtx = boost::add_vertex(g);
    auto vrtxPr = std::make_tuple(word, vrtx,weight,weightO);
    vertexWator.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(std::make_tuple(word,weight,weightO,"","","",position));
    posLast = position + word.size();
  }
  auto vrtxEnd = add_vertex(g);
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd,1.0,1.0);
  vertexWator.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back(std::make_tuple("E",1.0,1.0,"","","",-1));
 
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
      //auto weightOld = boost::get(boost::edge_weight_t(), g, ed.first);
      boost::put(boost::edge_weight_t(), g, ed.first, 0.0);
    }
  }
  
  DUMP_VAR3(boost::num_vertices(g),vrtxStart,vrtxEnd);


  
  
  
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto next = position + range;
    auto rangeSelf = vertexWator.equal_range(position);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto weightR = std::get<2>(itSelf->second);
      auto weightRO = std::get<3>(itSelf->second);
      if (word == wordSelf) {
        auto rangeNext = vertexWator.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second;
             itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          auto wordNext = std::get<0>(itNext->second);
          if(wordNext == "E") {
              continue;
          }
          string phrase = word + "-" + wordNext;
          auto phraseWeight = phraseInputCN_.getRangeMin()/dConstWeightNotFoundFator;
          if(lang =="ja") {
              phraseWeight = phraseInputJA_.getRangeMin()/dConstWeightNotFoundFator;
          }
          auto pred = -1.0;
          if(lang =="cn") {
            pred = phraseInputCN_.getDoublePred(phrase);
          }
          if(lang =="ja") {
            pred = phraseInputJA_.getDoublePred(phrase);
          }
          if (pred > 0.0) {
            phraseWeight = 1.0 / pred;
          } else {
            phraseWeight = 1.0 / phraseWeight;
          }
          auto ed = boost::add_edge(vrtxSelf, vrtxNext,g);
          boost::put(boost::edge_weight_t(), g, ed.first, phraseWeight);
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
          //auto weight = boost::get(boost::edge_weight_t(), g, ed.first);
          boost::put(boost::edge_weight_t(), g, ed.first, 0.0);
          break;
        }
      }
    }
  }
  DUMP_VAR3(boost::num_vertices(g),vrtxStart,vrtxEnd);
 
    
  sample_graph_writer gw(labelVertex);
  sample_graph_weight_writer gew(g);

  std::stringstream ss;
  boost::write_graphviz(ss, g, gw,gew);
  auto dotStr = ss.str();
  boost::algorithm::replace_all(
      dotStr, "digraph G {",
      "digraph G { \n rankdir=LR;\n graph [charset=\"UTF-8\"];\n");
  TRACE_VAR(dotStr);
  // dot -v -T svg 1.dot -o 1.svg
  
  
  
  DUMP_VAR3(boost::num_vertices(g),vrtxStart,vrtxEnd);
  
  std::vector<Vertex> parents(boost::num_vertices(g));
  std::vector<float> distance(boost::num_vertices(g));
  
  auto predmap = boost::predecessor_map(boost::make_iterator_property_map(parents.begin(), boost::get(boost::vertex_index, g))).
                          distance_map(boost::make_iterator_property_map(distance.begin(), boost::get(boost::vertex_index, g)));
  
  boost::dijkstra_shortest_paths(g,vrtxStart,predmap);
  
  
  #if 0
  boost::graph_traits < Graph >::vertex_iterator vi, vend;
  for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    auto labelSelf = std::get<0>(labelVertex.at(*vi));
    auto labelParent = std::get<0>(labelVertex.at(parents[*vi]));
    std::cout << "distance(" << labelSelf << ") = " << distance[*vi] << ", ";
    std::cout << "parent(" << labelSelf << ") = " << labelParent << std::endl;
  }
  #endif
  
  DUMP_VAR(distance.at(vrtxEnd));

  if (parents[vrtxEnd] == vrtxEnd) {
    std::cout << "no path" << std::endl;
    return ;
  }
  std::vector<int> path;
  for(auto v = vrtxEnd; v != vrtxStart; v = parents[v]) {
    if(v != vrtxEnd) {
      path.push_back(v);
    }
  }
  DUMP_VAR(path.size());
  for(auto it = path.rbegin();it != path.rend();it++) {
      auto wordSelected = std::get<0>(labelVertex.at(*it));
      DUMP_VAR2(*it,wordSelected);
      auto positionSelected = std::get<6>(labelVertex.at(*it));
      DUMP_VAR3(*it,wordSelected,positionSelected);
      for (auto elem : confuse) {
        auto word = std::get<0>(elem.second);
        auto position = std::get<1>(elem.second);
        if(word ==wordSelected && positionSelected == position) {
            wordSeqTopSelected_.insert(elem);
            break;
        }
      }
  }
}


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

string ZhiZiWord::createGraphPhrase(const string &text,const string &sentence,const string &lang) {
  Graph g;
  multimap<int, std::tuple<string, Vertex,double,double>> vertexWator;
  vector<LabelVertex> labelVertex;
  
  auto vrtxStart = boost::add_vertex(g);
  auto vrtxPrStart = std::make_tuple("S", vrtxStart,1.0,1.0);
  vertexWator.insert(std::make_pair(-1, vrtxPrStart));
  labelVertex.push_back(std::make_tuple("S",1.0,1.0,"","","",-1));
 
  int posLast = 0;
  for (auto elem : wordAdjustedSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto weight = std::get<3>(elem.second);
    auto weightO = std::get<4>(elem.second);
    auto vrtx = boost::add_vertex(g);
    auto vrtxPr = std::make_tuple(word, vrtx,weight,weightO);
    vertexWator.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(std::make_tuple(word,weight,weightO,"","","",position));
    posLast = position + word.size();
  }
  auto vrtxEnd = add_vertex(g);
  auto vrtxPrvrtxEnd = std::make_tuple("E", vrtxEnd,1.0,1.0);
  vertexWator.insert(std::make_pair(posLast,vrtxPrvrtxEnd));
  labelVertex.push_back(std::make_tuple("E",1.0,1.0,"","","",-1));

  auto vrtxTitle = add_vertex(g);
  auto title = u8"input=【" + text + u8"】";
  auto vrtxPrvrtxTitle = std::make_tuple(title, vrtxTitle,0.0,0.0);
  vertexWator.insert(std::make_pair(posLast + 10,vrtxPrvrtxTitle));
  labelVertex.push_back(std::make_tuple(title,1.0,1.0,"distance : 1.0 / adjusted duplicate rate"
                                        ," adjusted duplicate rate:  duplicate rate* power(CONST, word length)"
                                        ,"duplicate rate : this word duplicate couter / max duplicate couter",-1));

  auto vrtxSentence = add_vertex(g);
  auto titleSentence = u8"output=【" + sentence + u8"】";
  auto vrtxPrvrtxvrtxSentence = std::make_tuple(titleSentence, vrtxSentence,0.0,0.0);
  vertexWator.insert(std::make_pair(posLast + 11,vrtxPrvrtxvrtxSentence));
  labelVertex.push_back(std::make_tuple(titleSentence,1.0,1.0,"---"
                                        ,"---"
                                        ,"---",-1));
    
  auto ed = boost::add_edge(vrtxTitle,vrtxSentence,g);

    
  // add dummy start.
  {
    auto startPos = wordAdjustedSeq_.begin()->first;
    auto rangeSelf = vertexWator.equal_range(startPos);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto weightR = std::get<2>(itSelf->second);
      auto weightRO = std::get<3>(itSelf->second);
      auto ed = boost::add_edge(vrtxStart, vrtxSelf,g);
      boost::put(boost::edge_weight_t(), g, ed.first, 0.0);
    }
  }    
    
  for (auto elem : wordAdjustedSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto next = position + range;
    auto rangeSelf = vertexWator.equal_range(position);
    for (auto itSelf = rangeSelf.first; itSelf != rangeSelf.second; itSelf++) {
      auto wordSelf = std::get<0>(itSelf->second);
      auto vrtxSelf = std::get<1>(itSelf->second);
      auto weightR = std::get<2>(itSelf->second);
      auto weightRO = std::get<3>(itSelf->second);
      if (word == wordSelf) {
        auto rangeNext = vertexWator.equal_range(next);
        for (auto itNext = rangeNext.first; itNext != rangeNext.second;
             itNext++) {
          auto vrtxNext = std::get<1>(itNext->second);
          auto ed = boost::add_edge(vrtxSelf, vrtxNext,g);
          auto wordNext = std::get<0>(itNext->second);
          if(wordNext == "E") {
              continue;
          }
          string phrase = word + "-" + wordNext;
          auto phraseWeight =  phraseInputCN_.getRangeMin()/dConstWeightNotFoundFator;
          if(lang =="ja") {
              phraseWeight = phraseInputJA_.getRangeMin()/dConstWeightNotFoundFator;
          }
          auto pred = -1.0;
          if(lang =="cn") {
            pred = phraseInputCN_.getDoublePred(phrase);
          }
          if(lang =="ja") {
            pred = phraseInputJA_.getDoublePred(phrase);
          }
          if (pred > 0.0) {
              phraseWeight = 1.0 / pred;
          } else {
              phraseWeight = 1.0 / phraseWeight;
          }
          TRACE_VAR(phrase,pred,phraseWeight);
          boost::put(boost::edge_weight_t(), g, ed.first, phraseWeight);
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
          boost::put(boost::edge_weight_t(), g, ed.first, 0.0);
          break;
        }
      }
    }
  }
  
    
  sample_graph_writer gw(labelVertex);
  sample_graph_weight_writer gew(g);

  std::stringstream ss;
  boost::write_graphviz(ss, g, gw,gew);
  auto dotStr = ss.str();
  boost::algorithm::replace_all(
      dotStr, "digraph G {",
      "digraph G { \n rankdir=LR;\n graph [charset=\"UTF-8\"];\n");
  //DUMP_VAR(dotStr);
  
  int prePost = -2;
  string oneSame;
  string allSame;
  int elemPost = 1;
  for (auto elem : wordAdjustedSeq_) {
      auto position = std::get<1>(elem.second);
      if(position == prePost) {
          oneSame += ";";
          oneSame += to_string(elemPost);
      } else {
          if(oneSame.empty()==false) {
              oneSame += "}\n";
              allSame += oneSame;
          }
          oneSame = "{rank = same;";
          oneSame += to_string(elemPost);
      }
      prePost = position;
      elemPost++;
  }
  TRACE_VAR(allSame);
  boost::algorithm::replace_all(
      dotStr, "}",
      allSame +"}");
  TRACE_VAR(dotStr);

  /*{rank = same; A; X;}*/
  
  auto id = boost::uuids::random_generator()();
  auto fileName = boost::lexical_cast<std::string>(id);
  ::system("mkdir -p /tmp/wator/wai/graph/");
  string pathDot = "/tmp/wator/wai/graph/" + fileName +".dot";
  std::ofstream outfile(pathDot,std::ofstream::binary);
  if(outfile.good()) {
    outfile << dotStr;
    outfile.close();
  }
  ::system("mkdir -p /watorvapor/autogen/wator/wai/graph/");
  {
      string dotCmd = "dot -v -T png ";
      dotCmd += pathDot;
      dotCmd += " -o ";
      dotCmd += "/watorvapor/autogen/wator/wai/graph/" + fileName +".png";
      dotCmd += " >/dev/null 2>&1";
      ::system(dotCmd.c_str());
      // dot -v -T png 1.dot -o 1.png
  }
  {
      string dotCmd = "dot -v -T svg ";
      dotCmd += pathDot;
      dotCmd += " -o ";
      dotCmd += "/watorvapor/autogen/wator/wai/graph/" + fileName +".svg";
      dotCmd += " >/dev/null 2>&1";
      ::system(dotCmd.c_str());
      // dot -v -T svg 1.dot -o 1.svg
  }

    
    string url("/autogen/wai/graph/");
    url += fileName +"";
    return url;
}
