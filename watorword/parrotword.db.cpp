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

#include "log.hpp"
#include "parrotword.hpp"



map<string, int> ParrotWord::multiWordOfOneArticle_;


bool ParrotWord::loadMaster(bool forceCast) {
  if(dictInputCN_.loadMasterFromDB(database_+"/cn") == false) {
    return false;
  }
  if(dictInputJA_.loadMasterFromDB(database_+"/ja") == false) {
    return false;
  }
  return true;
}

void ParrotWord::unloadMaster(void) {
  dictInputCN_.unloadMasterFromDB();
  dictInputJA_.unloadMasterFromDB();
}



void ParrotWord::collectWord(void) {
  for (auto word : prediWords_) {
    auto it = multiWordOfOneArticle_.find(word);
    if (it != multiWordOfOneArticle_.end()) {
      it->second++;
    } else {
      multiWordOfOneArticle_[word] = 1;
    }
  }
}

vector<string> ParrotWord::pickupWordRanking(void) {
  DUMP_VAR(multiWordOfOneArticle_.size());
  vector<string> wordArrays;
  string upWords;
  int iCounter = 1;

  for (auto wordSum : multiWordOfOneArticle_) {
    TRACE_VAR(wordSum.first, wordSum.second);
    if (wordSum.second >= minWordRepeateTimes_) {
      TRACE_VAR(wordSum.first, wordSum.second);
      upWords += "{";
      upWords += wordSum.first;
      upWords += ",";
      upWords += std::to_string(wordSum.second / minWordRepeateTimes_);
      upWords += "};";
      iCounter++;
      if (iCounter % 256 == 0) {
        upWords += "{}";
        wordArrays.push_back(upWords);
        upWords.clear();
      }
    }
  }
  if (upWords.empty() == false) {
    upWords += "{}";
    wordArrays.push_back(upWords);
  }
  if (multiWordOfOneArticle_.empty()) {
    upWords += "{}";
    wordArrays.push_back(upWords);
  }
  return wordArrays;
}

void ParrotWord::commitArticle(const pt::ptree& task,const string &ws) {
  auto wordArrays = pickupWordRanking();
  multiWordOfOneArticle_.clear();
  ap_ = 0;
  for (auto word : wordArrays) {
    DUMP_VAR(word);
    pt::ptree upTask = task;
    upTask.put("word", word);
    string task_word_upPath = ws + "/task_word_up.json";
    pt::write_json(task_word_upPath, upTask);

    auto tagOpt = task.get_optional<string>("tag");
    if (tagOpt) {
      auto tag = tagOpt.get();
      string wgetTaskUp("curl -6 -F \"");
      // string wgetTaskUp("curl -F \"");
      wgetTaskUp += "file=@";
      wgetTaskUp += task_word_upPath;
      wgetTaskUp += "\" ";
      wgetTaskUp += "\"https://www.wator.xyz/wai/text/train/parrot/";
      wgetTaskUp += tag;
      wgetTaskUp += "\"";
      DUMP_VAR(wgetTaskUp);
      ::system(wgetTaskUp.c_str());
    }
  }
}

void ParrotWord::commitArticle(void) {
  multiWordOfOneArticle_.clear();
  ap_ = 0;
}

void ParrotWord::dumpRank() {
  for (auto rPair : statisticsRank_) {
    auto word = rPair.first;
    auto weight = std::get<0>(rPair.second);
    auto weight_orig = std::get<1>(rPair.second);
    DUMP_VAR4(word, word.size(), weight, weight_orig);
  }
}

void ParrotWord::dumpSeq() {
  for (auto elem : wordHintSeq_) {
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

void ParrotWord::dumpDot(void) {
  Graph g;
  multimap<int, std::tuple<string, Vertex>> vertexs;
  static vector<string> labelVertex;
  for (auto elem : wordHintSeq_) {
    auto word = std::get<0>(elem.second);
    auto position = std::get<1>(elem.second);
    auto vrtx = g.add_vertex();
    auto vrtxPr = std::make_tuple(word, vrtx);
    vertexs.insert(std::make_pair(position, vrtxPr));
    labelVertex.push_back(word);
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
