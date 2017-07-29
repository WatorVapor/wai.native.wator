#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <thread>
#include <vector>
using namespace std;

#define DUMP_VAR(x)                                                            \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"   \
            << std::endl;
#define DUMP_VAR2(x, y)                                                        \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "=<"  \
            << x << "," << y << ">" << std::endl;
#define DUMP_VAR3(x, y, z)                                                     \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << ","   \
            << #z << "=<" << x << "," << y << "," << z << ">" << std::endl;
#define DUMP_VAR4(x, y, z, a)                                                  \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << ","   \
            << #z << "," << #a << "=<" << x << "," << y << "," << z << ","     \
            << a << ">" << std::endl;
#define DUMP_VAR5(x, y, z, a, b)                                               \
  std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << ","   \
            << #z << "," << #a << "," << #b << "=<" << x << "," << y << ","    \
            << z << "," << a << "," << b << ">" << std::endl;

#define TRACE_VAR(...)

#include "treeword.hpp"

map<int, PredictionTree::WordElement> PredictionTree::uniqueWords_;
int PredictionTree::uniqIndex_ = 0;
vector<shared_ptr<PredictionTree>> PredictionTree::leaves_;

PredictionTree::PredictionTree() {
  TRACE_VAR(root_.use_count());
  uniqueWords_.clear();
  leaves_.clear();
}

bool PredictionTree::setWordSeq(
    const multimap<int, WordElement> &clearWordSeq) {
  multimap<int, WordElement> indexWordSeq;
  for (auto keyIt : clearWordSeq) {
    auto word = std::get<0>(keyIt.second);
    auto pos = std::get<1>(keyIt.second);
    auto range = std::get<2>(keyIt.second);
    auto weight = std::get<3>(keyIt.second);
    auto weight_orig = std::get<4>(keyIt.second);
    TRACE_VAR(word, pos, range, weight, weight_orig);
    uniqueWords_[uniqIndex_] = keyIt.second;
    auto indexElem =
        std::make_tuple(word, pos, range, weight, weight_orig, uniqIndex_);
    indexWordSeq.insert(std::make_pair(keyIt.first, indexElem));
    uniqIndex_++;
  }
  TRACE_VAR(root_.use_count());
  auto myPtr = shared_from_this();
  TRACE_VAR(myPtr.use_count());
  auto rootElem = indexWordSeq.begin();
  if (rootElem == indexWordSeq.end()) {
    return true;
  }
  TRACE_VAR(rootElem->first);
  return this->setWordSeq(indexWordSeq, rootElem->first);
  /*
    if(result == false) {
       DUMP_VAR(result);
       uniqueWords_.clear();
       leaves_.clear();
       nodes_.clear();
       vector<multimap<int,WordElement>> splitedSeq;
       this->splitComplexWordSeq(indexWordSeq,splitedSeq);
       DUMP_VAR(splitedSeq.size());
       for(auto seq:splitedSeq) {
          this->setWordSeq(seq);
       }
       DUMP_VAR2(leaves_.size(),nodes_.size());
    }
  */
  // this->dumpAllPreds();
}

static int iConstLeavesWidthMax = 1024;

bool PredictionTree::setWordSeq(const multimap<int, WordElement> &indexWordSeq,
                                int pos) {
  if (leaves_.size() > iConstLeavesWidthMax) {
    DUMP_VAR2(leaves_.size(), iConstLeavesWidthMax);
    return false;
  }
  auto myPtr = shared_from_this();
  TRACE_VAR(myPtr.use_count());
  TRACE_VAR(pos);
  auto sameKey = indexWordSeq.equal_range(pos);
  int iSameKeyCounter = 0;
  for (auto keyIt = sameKey.first; keyIt != sameKey.second; keyIt++) {
    auto word = std::get<0>(keyIt->second);
    auto pos = std::get<1>(keyIt->second);
    auto range = std::get<2>(keyIt->second);
    auto weight = std::get<3>(keyIt->second);
    auto weight_orig = std::get<4>(keyIt->second);
    auto index = std::get<5>(keyIt->second);
    TRACE_VAR(word, pos, range, weight, index);
    auto predNode = std::make_shared<PredictionTree>(myPtr, index);
    myPtr->nodes_.push_back(predNode);
    if (pos + range == pos) {
      DUMP_VAR2(pos, pos);
    }
    auto retSub = predNode->setWordSeq(indexWordSeq, pos + range);
    if (retSub == false) {
      return false;
    }
    iSameKeyCounter++;
  }
  TRACE_VAR(iSameKeyCounter);
  if (iSameKeyCounter == 0) {
    auto ir = uniqueWords_.find(this->index_);
    if (ir != uniqueWords_.end()) {
      auto word = std::get<0>(ir->second);
      TRACE_VAR(pos, word, iSameKeyCounter);
    } else {
      TRACE_VAR(pos, iSameKeyCounter);
    }
    leaves_.push_back(myPtr);
  }
  TRACE_VAR(this->nodes_.size());
  return true;
}

PredictionTree::PredictionTree(shared_ptr<PredictionTree> root, int index)
    : root_(root), index_(index) {}

PredictionTree::~PredictionTree() { TRACE_VAR(root_.use_count()); }

void PredictionTree::splitComplexWordSeq(
    const multimap<int, WordElement> &clearWordSeq,
    vector<multimap<int, WordElement>> &splitedSeq) {
  auto topIt = clearWordSeq.begin();
  double topWeight = 0;
  for (auto it = clearWordSeq.begin(); it != clearWordSeq.end(); it++) {
    auto weight = std::get<3>(it->second);
    if (weight > topWeight) {
      topWeight = weight;
      topIt = it;
    }
    auto word = std::get<0>(it->second);
    auto pos = std::get<1>(it->second);
    auto range = std::get<2>(it->second);
    auto weight_orig = std::get<4>(it->second);
    TRACE_VAR(word, pos, range, weight, weight_orig);
  }
  multimap<int, WordElement> newSeq;
  auto itStart = clearWordSeq.begin();
  auto itStop = topIt;
  auto posTop = std::get<1>(topIt->second);
  auto rangeTop = std::get<2>(topIt->second);
  while (itStop != clearWordSeq.begin()) {
    auto posPrev = std::get<1>(itStop->second);
    if (posTop > posPrev) {
      break;
    }
    itStop = std::prev(itStop);
  }
  if (itStop != clearWordSeq.end()) {
    multimap<int, WordElement> sepBefore(clearWordSeq.begin(), itStop);
    splitedSeq.push_back(sepBefore);
    multimap<int, WordElement> sepSelf = {{topIt->first, topIt->second}};
    splitedSeq.push_back(sepSelf);
  }
  auto nextIt = topIt;
  auto nextPosStop = posTop + rangeTop;
  while (nextIt++ != clearWordSeq.end()) {
    auto posNext = std::get<1>(nextIt->second);
    if (posNext >= nextPosStop) {
      break;
    }
  }
  if (++nextIt != clearWordSeq.end()) {
    multimap<int, WordElement> sepAfter(nextIt, clearWordSeq.end());
    splitedSeq.push_back(sepAfter);
  }
}

void PredictionTree::flatPredSeq(multimap<int, vector<WordElement>> &flat,
                                 double &weightSum) {
  if (leaves_.size() > iConstLeavesWidthMax) {
    return;
  }
  multimap<int, vector<WordElement>> flatDummy;
  double weightMaxDummy = 0.0;
  this->flatPredSeqInside(flatDummy, weightMaxDummy);
  TRACE_VAR(flatDummy.size(), weightMaxDummy);
  weightSum = 0.0;
  for (auto dummy : flatDummy) {
    //    std::cout << "################" << std::endl;
    auto predWeightSum = 0.0;
    for (auto elem : dummy.second) {
      auto word = std::get<0>(elem);
      auto pos = std::get<1>(elem);
      auto range = std::get<2>(elem);
      auto weight = std::get<3>(elem);
      auto weight_orig = std::get<4>(elem);
      TRACE_VAR(word, pos, range, weight, weight_orig);
      predWeightSum += weight;
    }
    if (dummy.second.empty() == false) {
      auto divPredWeight = (double)predWeightSum / (double)dummy.second.size();
      TRACE_VAR(divPredWeight);
      double divPredWeight2 =
          ((double)(UINT16_MAX)*divPredWeight) / weightMaxDummy;
      int divPredWeight3 = divPredWeight2;
      weightSum += divPredWeight2;
      TRACE_VAR(divPredWeight, divPredWeight2, divPredWeight3);
      flat.insert(std::make_pair(divPredWeight3, dummy.second));
    }
    //    std::cout << "***************" << std::endl;
  }
  /*
    for(auto seqPred:flat){
      //std::cout << "-------------" << std::endl;
      auto fredFloat = (double)(seqPred.first)/(double)(weightSum);
      TRACE_VAR(fredFloat);
      for(auto elem:seqPred.second) {
        auto word = std::get<0>(elem);
        auto pos = std::get<1>(elem);
        auto range = std::get<2>(elem);
        auto weight = std::get<3>(elem);
        auto weight_orig = std::get<4>(elem);
        TRACE_VAR(word,pos,range,weight,weight_orig);
      }
      //std::cout << "++++++++++++++" << std::endl;
    }
  */
}

void PredictionTree::flatPredSeqInside(multimap<int, vector<WordElement>> &flat,
                                       double &weightMax) {
  TRACE_VAR(leaves_.size());
  for (auto leaf : leaves_) {
    static int dummyPredSum = 0;
    vector<WordElement> seq;
    leaf->getPredSeq(seq, weightMax);
    TRACE_VAR(seq.size());
    std::reverse(std::begin(seq), std::end(seq));
    flat.insert(std::make_pair(dummyPredSum++, seq));
  }
}
void PredictionTree::getPredSeq(vector<WordElement> &seq, double &weightMax) {
  TRACE_VAR(this->root_.use_count());
  if (this->root_.expired() == false) {
    auto it = uniqueWords_.find(this->index_);
    if (it != uniqueWords_.end()) {
      seq.push_back(it->second);
      auto word = std::get<0>(it->second);
      auto weight = std::get<3>(it->second);
      TRACE_VAR(word, weight);
      if (weight > weightMax) {
        weightMax = weight;
      }
    } else {
      TRACE_VAR(false, this->index_);
      for (auto words : uniqueWords_) {
        DUMP_VAR2(words.first, this->index_);
      }
    }
    auto rootNode = this->root_.lock();
    rootNode->getPredSeq(seq, weightMax);
  } else {
    TRACE_VAR(this->root_.expired(), this->root_.use_count());
  }
}

void PredictionTree::dumpAllPreds(void) {
  int depth = 0;
  auto ir = uniqueWords_.find(this->index_);
  if (ir != uniqueWords_.end()) {
    auto word = std::get<0>(ir->second);
    auto pos = std::get<1>(ir->second);
    auto range = std::get<2>(ir->second);
    auto weight = std::get<3>(ir->second);
    auto weight_orig = std::get<4>(ir->second);
    DUMP_VAR5(word, pos, range, weight, weight_orig);
  }
  if (root_.expired()) {
    DUMP_VAR("Top");
  }
  for (auto node : nodes_) {
    node->dumpAllPreds(depth);
  }
}
void PredictionTree::dumpAllPreds(int depth) {
  auto ir = uniqueWords_.find(this->index_);
  if (ir != uniqueWords_.end()) {
    auto word = std::get<0>(ir->second);
    auto pos = std::get<1>(ir->second);
    auto range = std::get<2>(ir->second);
    auto weight = std::get<3>(ir->second);
    auto weight_orig = std::get<4>(ir->second);
    DUMP_VAR5(word, pos, range, weight, depth);
  }
  if (nodes_.empty()) {
    DUMP_VAR(nodes_.empty())
  }
  for (auto node : nodes_) {
    node->dumpAllPreds(depth + 1);
  }
}
