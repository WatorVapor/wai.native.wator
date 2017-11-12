#include <iostream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
#pragma once

class CtrlClaw {
 public:
  CtrlClaw();
  ~CtrlClaw();
  void claw(const string &article);
  void cut(const string &article);

  // loop
  template <typename T>
  void eachSentence(T fn) {
    for (auto pair : sentence_words_) {
      auto strWord = std::get<0>(pair);
      auto word = std::get<1>(pair);
      auto multi = std::get<2>(pair);
      if(multi) {
        fn(strWord, word);
      }
    }
  }
  // loop
  template <typename T>
  void eachSentenceMix(T fn) {
    for (auto pair : sentence_words_) {
      auto strWord = std::get<0>(pair);
      auto word = std::get<1>(pair);
      auto multi = std::get<2>(pair);
      fn(strWord, word,multi);
    }
  }
  // loop multi bytes
  template <typename T>
  void eachMultiByte(T fn) {
    for (auto pair : sentence_words_) {
      auto word = std::get<1>(pair);
      fn(word);
    }
  }

 private:
  void clear(void);
  bool isOneByte(const string &utf8);
  bool isTwoByte(const string &utf8);
  bool isCtrlMultiByte(const string &utf8);

 private:
  string article_;
  vector<string> sentences_;
  vector<tuple<string, vector<string>,bool>> sentence_words_;
  vector<string> words_;
  vector<int> positions_;
  const map<string, bool> gCtrlMultiByteWord = {{u8"“", true},
                                                {u8"”", true},
                                                {u8"（", true},
                                                {u8"）", true},
                                                {u8"《", true},
                                                {u8"》", true},
                                                {u8"「", true},
                                                {u8"」", true},
                                                {u8"〔", true},
                                                {u8"〕", true},
                                                {u8"『", true},
                                                {u8"』", true},
                                                //{u8"–",true},{u8"—",true},
                                                {u8"　", true},
                                                {u8"…", true},
                                                {u8"?", true},
                                                {u8"·", true},
                                                {u8"、", true},
                                                {u8"。", true},
                                                {u8"，", true},
                                                {u8"．", true},
                                                {u8"【", true},
                                                {u8"】", true},
                                                {u8"〖", true},
                                                {u8"〖", true},
                                                {u8"?", true},
                                                {u8"★", true},
                                                {u8"：", true},
                                                {u8"？", true},
                                                {u8"；", true},
                                                {u8"！", true},
                                                {u8"×", true},
                                                {u8"2", true},
                                                {u8"○", true},
                                                {u8"●", true},
                                                {u8"※", true},
                                                {u8"→", true},
                                                {u8"◇", true},
                                                {u8"◎", true},
                                                {u8"☆", true},
                                                {u8"】", true},
                                                {u8"“", true},
                                                {u8"▲", true},
                                                {u8"≥", true},
                                                {u8"ê", true},
                                                {u8"’", true},
                                                {u8"ì", true},
                                                {u8"←", true},
                                                {u8"!", true},
                                                {u8"～", true},
                                                {u8"〉", true},
                                                {u8"〒", true},
                                                {u8"&", true},

                                                {u8"♥", true},
                                                {u8"℃", true},
                                                {u8"⇒", true},
                                                {u8"—", true},
                                                {u8"■", true},
                                                {u8"", true}};
};
