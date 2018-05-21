
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include "ipfspump.hpp"

IpfsTextPump::IpfsTextPump() : 
resoureBlock_("") {
  auto uuid = boost::uuids::random_generator()();
  ws_ = "/tmp/wai.native/" + boost::lexical_cast<std::string>(uuid);
  string cmd = "mkdir -p ";
        cmd += ws_;
  ::system(cmd.c_str());
}
IpfsTextPump::~IpfsTextPump() {}



#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;

bool IpfsTextPump::fetchMasterTask(pt::ptree &task, string &content) {
  if(resoureBlock_.empty()) {
    return;
  }
  
  string taskJSONPath = ws_ + "/task.json";
  string taskTextPath = ws_ + "/task.text";
  string wget = "wget -6 --tries=3 --connect-timeout=10 \"";
  // string wget =  "wget ";
  wget += url_;
  wget += "\" -O ";
  wget += taskJSONPath;
  DUMP_VAR(wget);
  ::system(wget.c_str());
  try {
    pt::ptree taskJson;
    pt::read_json(taskJSONPath, taskJson);
    task = taskJson;
    string taskURL;
    auto taskURLOpt = taskJson.get_optional<string>("url");
    if (taskURLOpt) {
      taskURL = taskURLOpt.get();
      DUMP_VAR(taskURL);
      string textWget = "wget --tries=3 --connect-timeout=10 \"";
      textWget += taskURL;
      textWget += "\" -O ";
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
            pt::write_xml(ss,textXml,std::locale(),pt::xml_writer_make_settings('
         ', indent, widen<char>("utf-8")));
            content = ss.str();
      */
    }
  } catch (const pt::json_parser::json_parser_error &e) {
    DUMP_VAR(e.what());
    return false;
  } catch (const std::exception &ex) {
    DUMP_VAR(ex.what());
    return false;
  }
  return true;
}
