#include <string>
#include <tuple>
#include <map>
#include <vector>
#include <memory>
#include <cstdint>
using namespace std;
#pragma once


class PredictionTree : public std::enable_shared_from_this<PredictionTree> {
  typedef std::tuple<std::string,int,int,double,double,int> WordElement;
public:
  PredictionTree();
  PredictionTree(shared_ptr<PredictionTree> root,int index);
  ~PredictionTree();
  bool setWordSeq(const multimap<int,WordElement> &clearWordSeq);
  void splitComplexWordSeq(const multimap<int,WordElement> &clearWordSeq,vector<multimap<int,WordElement>> &splitedSeq);
  void dumpAllPreds(void);
  // {pred,vector of words}
  void flatPredSeq(multimap<int,vector<WordElement>> &flat,double &weightSum);
private:
  bool setWordSeq(const multimap<int,WordElement> &clearWordSeq,int pos);
  void flatPredSeqInside(multimap<int,vector<WordElement>> &flat,double &weightMax);
  void getPredSeq(vector<WordElement> &seq,double &weightMax);
  void dumpAllPreds(int depth);
private:
  weak_ptr<PredictionTree> root_;
  vector<shared_ptr<PredictionTree>> nodes_;
  int index_;

  static map<int,WordElement> uniqueWords_;
  static vector<shared_ptr<PredictionTree>> leaves_;
  static int uniqIndex_;
};
