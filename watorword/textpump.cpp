
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>
using namespace std;

#include "textpump.hpp"

TextPump::TextPump(){
}
TextPump::TextPump(const string &dir) 
: dir_(dir){
}
TextPump::TextPump(const string &url,const string &tag) 
: dir_("")
,url_(url)
,tag_(tag){
}
TextPump::~TextPump() {
}
string TextPump::statistics(void) {
  std::stringstream ss;
  ss << "iTototl=<" << iTototl << ">,iProccessed =<" << iProccessed  << ">";
  return ss.str();
}
