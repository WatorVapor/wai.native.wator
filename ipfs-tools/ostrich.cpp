#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include "log.hpp"

#include "ctrlclaw.hpp"
#include "ostrichword.hpp"
#include "ipfspump.hpp"

static string longTxt(
    u8"它是由从地方到全球范围内几百万个私人的、学术界的、"
    u8"企业的和政府的网络所构成，通过电子，"
    u8"企业的和政府的网络所构成，通过<>"
    u8"无线和光纤网络技术等等<>一系列广泛的技术联系在一起。"
    u8"这种将计算机网络互相联接在一起的方法可称作“网络互联”，"
    u8"在这基础上发展出覆盖全世界的全球性互联网络称互联网，"
    u8"即是互相连接一起的网络。");

static string shortTxt(
    u8"人造卫星能够成功执行预定任务，单凭卫星本身是不行的，"
    u8"而需要完整的卫星工程系统");

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
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;




int main(int ac, char *av[]) {
  OstrichWord ostrich;
  auto learnOstrich = [&](const vector<string> &words) {
    ostrich.learn(words);
  };
  CtrlClaw claw;
  auto clawText = [&](const json &task, string &content,string &ws) {
    TRACE_VAR(task);
    claw.claw(content);
    claw.eachMultiByte(learnOstrich);
    ostrich.commitArticleIpfs(task,ws);
  };
  IpfsTextPump txtPump;
  txtPump.eachText(clawText);
  return 0;
}
