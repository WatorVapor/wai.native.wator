#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

#include "textpump.hpp"
#include "ctrlclaw.hpp"
#include "qiangbaoword.hpp"
#include "log.hpp"



int main(int ac,char*av[])
{
  QiangbaoWord qiangbao("./db/wiki.ja");
  if(qiangbao.loadMaster()==false) {
    return 0;
  }
  auto learnQiangbao = [&](string wordStr, vector<string> word) {
    qiangbao.learn(word,wordStr);
  };

  CtrlClaw claw;
  auto clawText = [&](string &path,string &content) {
    DUMP_VAR(path);
    //DUMP_VAR(content);
    claw.claw(content);
    claw.eachSentence(learnQiangbao);
    qiangbao.commitArticle();
  };
  TextPump txtPump("./url_crawl/.wiki.ja.meta/");
  txtPump.eachNewText("qiangbao.debug.one",clawText);

  qiangbao.unloadMaster();
  return 0;
}
