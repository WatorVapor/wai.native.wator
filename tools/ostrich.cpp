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


void parseURL(const pt::ptree &task,string &content) {
  //DUMP_VAR(content);
  
  boost::regex r( "<a.*?href=['|\"](.*?)['|\"]" );
  auto startIte = content.begin();
  auto endIte = content.begin();
  boost::smatch result;
  int i= 1;
  while (boost::regex_search(startIte, endIte, result, r)) {
    //cout << i << " : " << result.str(1) << endl;
    DUMP_VAR2(i,result.str(1))
    startIte = result[0].second;
    i++;
  }
  
  namespace x = boost::xpressive;
  using x::s1;
  using x::_w;
  using x::_;
  x::mark_tag in_tag(1);
  x::mark_tag tag_name(2);
  x::sregex regex =
          "<"  >> (tag_name = +_w) >> ">"
               >> (in_tag = -*_) >>
          "</" >> tag_name >> ">";
  x::smatch what;
  if( x::regex_match(content, what, regex) ){
      std::cout << what[tag_name].str() << std::endl;
      std::cout << what[in_tag].str() << std::endl;
  }
}

