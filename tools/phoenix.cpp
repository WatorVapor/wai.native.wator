#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include "ctrlclaw.hpp"
#include "log.hpp"
#include "phoenixword.hpp"
#include "textpump.hpp"

vector<string> parseUTF8(const string &words);

void parseBytes(const vector<string> &Bytes, const string &text);

static string longTxt(
    u8"它是由从地方到全球范围内几百万个私人的、学术界的、"
    u8"企业的和政府的网络所构成，通过电子，"
    u8"无线和光纤网络技术等等一系列广泛的技术联系在一起。"
    u8"这种将计算机网络互相联接在一起的方法可称作“网络互联”，"
    u8"在这基础上发展出覆盖全世界的全球性互联网络称互联网，"
    u8"即是互相连接一起的网络。");

static string shortTxt(
    u8"人造卫星能够成功执行预定任务，单凭卫星本身是不行的，"
    u8"而需要完整的卫星工程系统");

static string oneTxt(u8"它就要背负着积累于人世间的所有不快和仇恨恩怨");

static string bugTxt(
    u8"，影片讲述了江湖三大高手之一的笑三少遭人陷害亡命天涯并"
    u8"追查事情真相的故事。");


/*
int main(int ac,char*av[])
{

  PhoenixWord phoenix("./db/phoenix");
  if(phoenix.loadMaster(false)==false) {
    return 0;
  }
  auto learnPhoenix = [&](string wordStr, vector<string> word) {
    phoenix.learn(word,wordStr,"cn");
    //phoenix.dumpDot();
  };

  CtrlClaw claw;
  claw.claw(longTxt);
  claw.eachSentence(learnPhoenix);
  phoenix.unloadMaster();
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
  CtrlClaw claw;
  PhoenixWord phoenix("./db/phoenix");
  if (phoenix.loadMaster(false) == false) {
    return 0;
  }
  auto clawText = [&](const pt::ptree &task, string &content,string &ws) {
    std::stringstream ssTask;
    pt::write_json(ssTask, task);
    DUMP_VAR(ssTask.str());
    // DUMP_VAR(content);
    auto langOpt = task.get_optional<string>("lang");
    if(langOpt) {
        auto lang = langOpt.get();
        claw.claw(content);
          auto learnPhoenix = [&](string wordStr, vector<string> word) {
            phoenix.learn(word, wordStr,lang);
          };
        claw.eachSentence(learnPhoenix);
    }
    phoenix.commitArticle(task,ws);
  };
  TextPump txtPump("https://www.wator.xyz/wai/text/train/phoenix", "tain.one");
  txtPump.eachTextFromMaster(clawText);
  phoenix.unloadMaster();
  return 0;
}

