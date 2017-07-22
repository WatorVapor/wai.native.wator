
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
TextPump::~TextPump() {
}
string TextPump::statistics(void) {
  std::stringstream ss;
  ss << "iTototl=<" << iTototl << ">,iProccessed =<" << iProccessed  << ">";
  return ss.str();
}
