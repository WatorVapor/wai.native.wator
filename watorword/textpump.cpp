
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

#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;

bool TextPump::fetchMasterTask(pt::ptree &task,string &content) {
  string taskJSONPath = "/tmp/wai.native/task.json";
  string taskTextPath = "/tmp/wai.native/task.text";
  string wget =  "wget -6 ";
  //string wget =  "wget ";
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
    task = taskJson;
    string taskURL;
    auto taskURLOpt = taskJson.get_optional<string>("url");
    if(taskURLOpt) {
      taskURL = taskURLOpt.get();
      DUMP_VAR(taskURL);
      string textWget = "wget ";
      textWget += taskURL;
      textWget += " -O ";
      textWget += taskTextPath;
      DUMP_VAR(textWget);
      ::system(textWget.c_str());
      std::ifstream textStream(taskTextPath);
      std::string str((std::istreambuf_iterator<char>(textStream)),
                   std::istreambuf_iterator<char>());
      textStream.close();
      content = str;
/*      
      pt::ptree textXml;
      pt::read_xml(taskTextPath,textXml);
      std::stringstream ss;
      const int indent = 2;
      pt::write_xml(ss,textXml,std::locale(),pt::xml_writer_make_settings(' ', indent, widen<char>("utf-8")));
      content = ss.str();
*/    
   }
  } catch (const pt::json_parser::json_parser_error& e) {
    DUMP_VAR(e.what());
  }
  catch( const std::exception & ex ) {
    DUMP_VAR(ex.what());
  }
  return true;
}

