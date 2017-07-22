#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <tuple>
#include <cmath>
using namespace std;

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define DUMP_VAR2(x,y) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "=<" << x << "," << y << ">" << std::endl;
#define DUMP_VAR3(x,y,z) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "=<" << x << "," << y << "," << z <<">" << std::endl;

#define DUMP_VAR4(x,y,z,a) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "=<" << x << "," << y << "," << z << "," << a <<">" << std::endl;

#define TRACE_VAR(...)


typedef std::tuple<std::string,int,int,int> WordElement;
typedef std::tuple<int,int> WordRankVect;

int getPred(const string &word);
static const int gWordLength = 32;
static const int gWeigthAdjustBase = 64;

void getRawRankPair(const vector<string> &Bytes,map<string,int> &rank,int & minWordSize);
void adjustRankPair(const map<string,int> &rank,map<string,int> &rankAdjust,int minWordSize);

void getRawRank(const vector<string> &Bytes,map<int,vector<string>> &rank,int & minWordSize);
void adjustRank(const map<int,vector<string>> &rank,map<int,vector<string>> &rankAdjust,int minWordSize);

void dumpRank(const map<int,vector<string>> &rank);
void cutTextByRank(vector<WordElement> &wordVector,const map<string,int> &rank,const string &text);
void cutTextByRank(multimap<int,WordElement> &wordSeq,const map<string,int> &rank,const string &text);


void dumpSeq(const multimap<int,WordElement> &wordSeq);

void getNoConflictSeq(const multimap<int,WordElement> &wordSeq,vector<int> &seq);
void showClearSeq(const string &text,const vector<int> &seq);
void getClearSeq(const multimap<int,WordElement> &wordSeq,const vector<int> &seq,vector<multimap<int,WordElement>> &elemSeq);
void dumpClearSeq(const vector<multimap<int,WordElement>> &elemSeq);

void calcWordPrediction(const vector<multimap<int,WordElement>> &elemSeq,vector<multimap<int,WordElement>> &predSeq);

void showWordPrediction(const string &text,const vector<multimap<int,WordElement>> &predSeq);
void getWordPrediction(const string &text,const vector<multimap<int,WordElement>> &predSeq,vector<string> &words);


void parseBytes(const vector<string> &Bytes,const string &text) {
  if(Bytes.empty()){
    return;
  }
  int minWordSize = 32;
  map<string,int> rank,rankAdjust;
  getRawRankPair(Bytes,rank,minWordSize);
  adjustRankPair(rank,rankAdjust,minWordSize);
  multimap<int,WordElement> wordSeq;
  cutTextByRank(wordSeq,rankAdjust,text);
  //dumpSeq(wordSeq);
  vector<int> noConflictSeq;
  getNoConflictSeq(wordSeq,noConflictSeq);
  showClearSeq(text,noConflictSeq);
  vector<multimap<int,WordElement>> elemSeq;
  getClearSeq(wordSeq,noConflictSeq,elemSeq);
  //dumpClearSeq(elemSeq);
  vector<multimap<int,WordElement>> predSeq;
  calcWordPrediction(elemSeq,predSeq);
  //dumpClearSeq(predSeq);
  showWordPrediction(text,predSeq);
  vector<string> parrotWords;
  getWordPrediction(text,predSeq,parrotWords);
  for(auto parrot:parrotWords) {
    std:cout << "'" << parrot << "',";
  }
  std::cout << std::endl;
}

void getRawRankPair(const vector<string> &Bytes,map<string,int> &rank,int & minWordSize) {
  list<string> preWords;
  DUMP_VAR(minWordSize);
  for(auto &word:Bytes) {
    /* multi */
    TRACE_VAR(word);
    auto it = preWords.rbegin();
    string jointWord(word);
    while(it!= preWords.rend()){
      jointWord = *it + jointWord;
      it++;
      TRACE_VAR(jointWord);
      auto pred = getPred(jointWord);
      if(pred > 0) {
        DUMP_VAR2(jointWord,pred);
        DUMP_VAR(jointWord.size());
        if(minWordSize > jointWord.size()){
          minWordSize = jointWord.size();
        }
        rank[jointWord] = pred;
      }
    }
    preWords.push_back(word);
    if(preWords.size() > gWordLength -1) {
      preWords.pop_front();
    }
  }
  DUMP_VAR(minWordSize);
}

void adjustRankPair(const map<string,int> &rank,map<string,int> &rankAdjust,int minWordSize) {
  for(auto rPair:rank){
    TRACE_VAR(rPair.first,rPair.second);
    auto word = rPair.first;
    auto weight = rPair.second;
    DUMP_VAR3(word,weight,word.size());
    double rate = (double)word.size()/(double)minWordSize - 1.0;
    double rate2 = ::pow(gWeigthAdjustBase,rate);
    TRACE_VAR(rate,rate2);
    auto weight_adj = weight *(int)rate2;
    DUMP_VAR4(word,word.size(),weight,weight_adj);
    rankAdjust[word] = weight_adj;
  }
}







void dumpRank(const map<int,vector<string>> &rank) {
  for(auto it = rank.rbegin();it != rank.rend();it++) {
    auto rankRepeat = it->first;
    for(auto word :it->second) {
      DUMP_VAR2(word,rankRepeat);
    }
  }
}

vector<size_t> find_all_substr(const string &sub,const string &text){
  vector<size_t> positions;
  auto pos = text.find(sub, 0);
  while(pos != string::npos)
  {
    positions.push_back(pos);
    pos = text.find(sub,pos+1);
  }
  return positions;
}


void cutTextByRank(vector<WordElement> &wordVector,const map<string,int> &rank,const string &text) {
  for(auto it = rank.begin();it != rank.end();it++) {
    auto rankRepeat = it->second;
    DUMP_VAR(rankRepeat);
    auto word = it->first;
    DUMP_VAR(word);
    auto posAll = find_all_substr(word,text);
    for(auto pos: posAll) {
      auto range = word.size();
      DUMP_VAR4(word,pos,range,rankRepeat);
      auto elem = std::make_tuple(word,pos,word.size(),rankRepeat);
      wordVector.push_back(elem);
    }
  }
}
void cutTextByRank(multimap<int,WordElement> &wordSeq,const map<string,int> &rank,const string &text) {
  for(auto it = rank.begin();it != rank.end();it++) {
    auto rankRepeat = it->second;
    DUMP_VAR(rankRepeat);
    auto word = it->first;
    DUMP_VAR(word);
    auto posAll = find_all_substr(word,text);
    for(auto pos: posAll) {
      auto range = word.size();
      DUMP_VAR4(word,pos,range,rankRepeat);
      auto elem = std::make_tuple(word,pos,word.size(),rankRepeat);
      wordSeq.insert(std::make_pair(pos,elem));
    }
  }
}
void dumpSeq(const multimap<int,WordElement> &wordSeq) {
  for(auto seq:wordSeq) {
    DUMP_VAR(seq.first);
    auto word = std::get<0>(seq.second);
    auto pos = std::get<1>(seq.second);
    auto range = std::get<2>(seq.second);
    auto weight = std::get<3>(seq.second);
    DUMP_VAR4(word,pos,range,weight);
  }
}

void getNoConflictSeq(const multimap<int,WordElement> &wordSeq,vector<int> &seq) {
#if 0
  for(auto seq:wordSeq) {
    DUMP_VAR(seq.first);
    auto word = std::get<0>(seq.second);
    auto pos = std::get<1>(seq.second);
    auto range = std::get<2>(seq.second);
    auto weight = std::get<3>(seq.second);
    DUMP_VAR4(word,pos,range,weight);
  }
#endif
  DUMP_VAR(wordSeq.size());
  auto it = wordSeq.begin();
  if(it == wordSeq.end()) {
    return;
  }
  auto prev = it;
  it++;
  int maxPreCover =0;
  while(it++ != wordSeq.end()) {
    auto word = std::get<0>(prev->second);
    auto pos = std::get<1>(prev->second);
    auto range = std::get<2>(prev->second);
    auto preCover = pos + range;
    if(maxPreCover < preCover) {
      maxPreCover = preCover;
    }
    auto nextPos = std::get<1>(it->second);
    DUMP_VAR2(maxPreCover,nextPos);
    if(maxPreCover <= nextPos) {
      DUMP_VAR4(word,pos,range,true);
      seq.push_back(nextPos);
      maxPreCover = 0;
    } else {
      DUMP_VAR4(word,pos,range,false);
    }
    prev = it;
  }
  DUMP_VAR(seq.size());
}

void showClearSeq(const string &text,const vector<int> &seq) {
  std::cout << std::endl;
  int start = 0;
  for(auto bt:seq) {
    auto sub = text.substr(start,bt -start);
    std::cout << sub << "%";
    start = bt;
  }
  if(start < text.size()) {
    auto sub = text.substr(start);
    std::cout << sub << "";
  }
  std::cout << std::endl;
}
void getClearSeq(const multimap<int,WordElement> &wordSeq,const vector<int> &seq,vector<multimap<int,WordElement>> &elemSeq){
  auto irBegin = wordSeq.begin();
  DUMP_VAR(seq.size());
  DUMP_VAR(elemSeq.size());
  for(auto bt:seq) {
    DUMP_VAR(bt);
    auto irBt = wordSeq.find(bt);
    multimap<int,WordElement> elems(irBegin,irBt);
    elemSeq.push_back(elems);
    irBegin = irBt;
  }
  if(irBegin != wordSeq.end()) {
    multimap<int,WordElement> elems(irBegin,wordSeq.end());
    elemSeq.push_back(elems);
  }
  DUMP_VAR(elemSeq.size());
}
void dumpClearSeq(const vector<multimap<int,WordElement>> &clearSeq) {
  for(auto seq:clearSeq) {
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    dumpSeq(seq);
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
}

void calcPrediction(const multimap<int,WordElement> &elemsEntangled,multimap<int,WordElement> &elemsTopSelected) {
  DUMP_VAR(elemsEntangled.size());
  multimap<int,WordElement> weightElem;
  for(auto elem :elemsEntangled){
    DUMP_VAR(elem.first);
    auto word = std::get<0>(elem.second);
    auto pos = std::get<1>(elem.second);
    auto range = std::get<2>(elem.second);
    auto weight = std::get<3>(elem.second);
    DUMP_VAR4(word,pos,range,weight);
    auto elemNew = std::make_tuple(word,pos,range,weight);
    weightElem.insert(std::make_pair(weight,elemNew));
  }
  auto lastPos = elemsEntangled.rbegin();
  auto firstPos = elemsEntangled.begin();
  auto place_length = lastPos->first - firstPos->first;
  place_length += std::get<2>(lastPos->second);
  DUMP_VAR(place_length);
  vector<bool> placeholder;
  for(int i = 0;i<place_length;i++){
    placeholder.push_back(false);
  }

  std::cout << "---------------" <<std::endl;
  auto place_begin = firstPos->first;
  
  
  for(auto itWeight = weightElem.rbegin();itWeight != weightElem.rend();itWeight++){
    DUMP_VAR(itWeight->first);
    auto word = std::get<0>(itWeight->second);
    auto pos = std::get<1>(itWeight->second);
    auto range = std::get<2>(itWeight->second);
    auto weight = std::get<3>(itWeight->second);
    DUMP_VAR4(word,pos,range,weight);

    auto offsetWord = pos - place_begin;
    bool hasPlaceFlag = true;
    for(int i = offsetWord;i< offsetWord + range;i++){
      if(placeholder.at(i)) {
        hasPlaceFlag = false;
        break;
      } else {
        placeholder[i] = true;
      }
    }
    DUMP_VAR(hasPlaceFlag);
    if(hasPlaceFlag) {
      elemsTopSelected.insert(std::make_pair(pos,itWeight->second));
    }
  }
  DUMP_VAR(elemsTopSelected.size());
}

void calcWordPrediction(const vector<multimap<int,WordElement>> &elemsSeq,vector<multimap<int,WordElement>> &predSeq) {
  for(auto elems:elemsSeq) {
    std::cout << "################" <<std::endl;
    multimap<int,WordElement> topSelected;
    calcPrediction(elems,topSelected);
    predSeq.push_back(topSelected);
    std::cout << "################" <<std::endl;
  }
}

const map<string,bool> gCtrlMultiByteWord = {
  {u8"“",true},{u8"”",true},{u8"（",true}, {u8"）",true},
  {u8"《",true},{u8"》",true}, {u8"「",true},{u8"」",true},
  {u8"〔",true},{u8"〕",true},{u8"『",true},{u8"』",true},
  {u8"–",true},{u8"—",true},
  {u8"　",true},
  {u8"…",true},{u8"?",true},{u8"·",true},{u8"、",true},{u8"。",true},
  {u8"，",true},{u8"．",true},{u8"【",true},{u8"】",true},
  {u8"〖",true},{u8"〖",true},{u8"?",true},
  {u8"★",true},{u8"：",true},{u8"？",true},{u8"；",true},
  {u8"！",true},{u8"■",true},
  {u8"",true}
};


void showWordPrediction(const string &text,const vector<multimap<int,WordElement>> &predSeq){
  vector<int> positions;
  for(auto seq:predSeq) {
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    for(auto elem:seq) {
      auto word = std::get<0>(elem.second);
      auto pos = std::get<1>(elem.second);
      auto range = std::get<2>(elem.second);
      auto weight = std::get<3>(elem.second);
      DUMP_VAR4(word,pos,range,weight);
      positions.push_back(pos);
      positions.push_back(pos + range);
    }
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
  DUMP_VAR(positions.size());
  auto last =  std::unique(positions.begin(), positions.end());
  DUMP_VAR(positions.size());
  positions.erase(last, positions.end()); 
  int start = 0;
  for(auto bt:positions) {
    auto sub = text.substr(start,bt -start);
    std::cout << sub << "%";
    start = bt;
  }
  if(start < text.size()) {
    auto sub = text.substr(start);
    std::cout << sub << "";
  }
  std::cout << std::endl;
}

void getWordPrediction(const string &text,const vector<multimap<int,WordElement>> &predSeq,vector<string> &words){
  vector<int> positions;
  for(auto seq:predSeq) {
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
    for(auto elem:seq) {
      auto word = std::get<0>(elem.second);
      auto pos = std::get<1>(elem.second);
      auto range = std::get<2>(elem.second);
      auto weight = std::get<3>(elem.second);
      DUMP_VAR4(word,pos,range,weight);
      positions.push_back(pos);
      positions.push_back(pos + range);
    }
    std::cout << "%%%%%%%%%%%%%%%%" <<std::endl;
  }
  DUMP_VAR(positions.size());
  auto last =  std::unique(positions.begin(), positions.end());
  positions.erase(last, positions.end()); 
  DUMP_VAR(positions.size());
  int start = 0;
  for(auto bt:positions) {
    auto length = bt -start;
    DUMP_VAR3(start,bt,length);
    auto sub = text.substr(start,length);
    words.push_back(sub);
    start = bt;
  }
  if(start < text.size()) {
    auto sub = text.substr(start);
    words.push_back(sub);
  }
}


