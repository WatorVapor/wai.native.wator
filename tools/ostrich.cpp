#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <streambuf>
using namespace std;

#include "log.hpp"

#include "textpump.hpp"
#include "ctrlclaw.hpp"
#include "ostrichword.hpp"


static string longTxt(
  u8"它是由从地方到全球范围内几百万个私人的、学术界的、"
  u8"企业的和政府的网络所构成，通过电子，"
  u8"企业的和政府的网络所构成，通过<>"
  u8"无线和光纤网络技术等等<>一系列广泛的技术联系在一起。"
  u8"这种将计算机网络互相联接在一起的方法可称作“网络互联”，"
  u8"在这基础上发展出覆盖全世界的全球性互联网络称互联网，"
  u8"即是互相连接一起的网络。"
);

static string shortTxt(u8"人造卫星能够成功执行预定任务，单凭卫星本身是不行的，而需要完整的卫星工程系统");

/*
int main(int ac,char*av[])
{
  OstrichWord ostrich("./db/baidu.baike");
  auto learnOstrich = [&](const vector<string> &words) {
    ostrich.learn(words);
  };
  auto dumpOstrich = [&](const string &sentence,const vector<string> &words) {
    DUMP_VAR(sentence);
    for(auto word:words) {
      DUMP_VAR(word);
    }
  };
  CtrlClaw claw;
  claw.claw(longTxt);
  claw.eachSentence(dumpOstrich);
  claw.eachMultiByte(learnOstrich);
  ostrich.mergeWordByArticle();

  claw.claw(shortTxt);
  claw.eachMultiByte(learnOstrich);
  ostrich.mergeWordByArticle();
  return 0;
}
*/

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;

void parseURL(const pt::ptree &task,string &content);
int main(int ac,char*av[])
{
  OstrichWord ostrich;
  auto learnOstrich = [&](const vector<string> &words) {
    ostrich.learn(words);
  };
  CtrlClaw claw;  
  auto clawText = [&](const pt::ptree &task,string &content) {
    std::stringstream ssTask;
    pt::write_json(ssTask,task);
    TRACE_VAR(ssTask.str());
    claw.claw(content);
    claw.eachMultiByte(learnOstrich);
    ostrich.upWordByArticle2Master(task);
    parseURL(task,content);
  };
  TextPump txtPump("https://www.wator.xyz/wai/text/train/ostrich","tain.one");
  txtPump.eachTextFromMaster(clawText);
  return 0;
}


#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include <boost/regex.hpp>

#include <regex>

const static string strMatchHref("href=\"");
void parseURL(const pt::ptree &task,string &content) {
  //DUMP_VAR(content);
  std::regex rx( "<a.*?href=['|\"](.*?)['|\"]" );
  string prefix;
  string base_url;
  try {
    auto prefixOpt = task.get_optional<string>("prefix");
    if(prefixOpt) {
      prefix = prefixOpt.get();
      DUMP_VAR(prefix);
    }
    auto base_urlOpt = task.get_optional<string>("base_url");
    if(base_urlOpt) {
      base_url = base_urlOpt.get();
      DUMP_VAR(base_url);
    }
  } catch (const pt::json_parser::json_parser_error& e) {
    DUMP_VAR(e.what());
  }
  catch( const std::exception & ex ) {
    DUMP_VAR(ex.what());
  }

  
  string crawlerArrays;
  for(auto it = std::sregex_iterator(content.begin(), content.end(), rx);
      it != std::sregex_iterator();
       ++it) {
    TRACE_VAR(it->position());
    std::smatch match = *it;
    auto match_href = match.str();
    TRACE_VAR(match_href);
    auto first = match_href.find(strMatchHref);
    auto last = match_href.find_last_of("\"");
    if(first != std::string::npos && last != std::string::npos && last > first){
      string href = match_href.substr (first + strMatchHref.size(),last-first -strMatchHref.size());
      TRACE_VAR(href);
      auto first = href.find(prefix);
      if(first == 0) {
        href = base_url + href;
        TRACE_VAR(prefix,href);
        crawlerArrays += "{";
        crawlerArrays += href;
        crawlerArrays += "};";
      }
    }
  }
  pt::ptree upTask(task);
  upTask.put("crawler",crawlerArrays);
  string task_url_upPath = "/tmp/wai.native/task_url_up.json";
  pt::write_json(task_url_upPath,upTask);
  string wgetTaskUp("curl -6 -F \"");
  //string wgetTaskUp("curl -F \"");
  wgetTaskUp += "file=@";
  wgetTaskUp += task_url_upPath;
  wgetTaskUp += "\" ";
  wgetTaskUp += "\"https://www.wator.xyz/wai/text/train/ostrich/url/tain.one\"";
  DUMP_VAR(wgetTaskUp);
}

