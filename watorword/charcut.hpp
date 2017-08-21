#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;
#pragma once

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

class CharCut {
 public:
  CharCut();
  ~CharCut();
  void pushContent(const vector<string> &input);
  void compressRanking(void);
  void dumpSum(void);

 private:
  bool isFiltOut(const string &utf8);
  bool isOneByte(const string &utf8);
  bool isCtrlMultiByte(const string &utf8);
  void pushMultiWord(const string &word, map<string, int> &wordSum);
  void pushMultiWordGlobal(const string &word, int counter);
  void pushOneWord(const string &word);
  vector<string> compressWordList(const vector<string> &words);
  bool matchWith(const string &word, vector<string> &words);
  void mergeContentWord(const map<string, int> &wordSum);
  void openDB();
  void closeDB();
  void writeDB();
  void dumpSnapshotDB();

 private:
  static map<string, int> gMultiWordSum;
  // static map<int,vector<string>> gMultiWordRank;
  static map<string, int> gOneWordSum;
  // static map<int,vector<string>> gOneWordRank;
  const int gWordLength = 32;
  leveldb::WriteBatch gSaveDBBatch;
  const map<string, bool> gCtrlMultiByteWord = {
      {u8"“", true},  {u8"”", true},  {u8"（", true}, {u8"）", true},
      {u8"《", true}, {u8"》", true}, {u8"「", true}, {u8"」", true},
      {u8"〔", true}, {u8"〕", true}, {u8"『", true}, {u8"』", true},
      {u8"–", true},  {u8"—", true},  {u8"　", true}, {u8"…", true},
      {u8"?", true},  {u8"·", true},  {u8"、", true}, {u8"。", true},
      {u8"，", true}, {u8"．", true}, {u8"【", true}, {u8"】", true},
      {u8"〖", true}, {u8"〖", true}, {u8"?", true},  {u8"★", true},
      {u8"：", true}, {u8"？", true}, {u8"；", true}, {u8"！", true},
      {u8"■", true},  {u8"", true}};
};
