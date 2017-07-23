
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

namespace pt = boost::property_tree;

bool TextPump::fetchMasterTask(pt::ptree &task,string &content) {
  string taskJSONPath = "/tmp/wai.native/result.json";
  string wget =  "wget -6 ";
  wget += url_; 
  wget += "/";
  wget += tag_;
  wget += " -O ";
  wget += taskJSONPath;
  DUMP_VAR(wget);
  ::system(wget.c_str());
  try {
    pt::ptree taskJson;
    pt::read_json(taskJSONPath, taskJson);
    string taskURL;
    auto taskURLOpt = taskJson.get_optional<string>("url");
    if(taskURLOpt) {
      taskURL = taskURLOpt.get();
    }
  } catch (const pt::json_parser::json_parser_error& e) {
    DUMP_VAR(e.what());
  }
  catch( const std::exception & ex ) {
    DUMP_VAR(ex.what());
  }
  return true;
}

