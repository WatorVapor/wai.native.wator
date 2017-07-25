#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <streambuf>
using namespace std;

#include "log.hpp"

#include "textpump.hpp"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;

#include "wikicrawler.hpp"


int main(int ac,char*av[])
{

  WikiCrawler clawler;  
  auto clawText = [&](const pt::ptree &task,string &content) {
    std::stringstream ssTask;
    pt::write_json(ssTask,task);
    TRACE_VAR(ssTask.str());
    clawler.parse(task,content);
  };
  TextPump txtPump("https://www.wator.xyz/wai/text/train/crawler","");
  txtPump.eachTextFromMaster(clawText);
  return 0;
}
