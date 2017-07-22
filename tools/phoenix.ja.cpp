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
#include "phoenixword.hpp"
#include "log.hpp"



int main(int ac,char*av[])
{
  PhoenixWord phoenix("./db/wiki.ja");
  if(phoenix.loadMaster()==false) {
    return 0;
  }
  auto learnPhoenix = [&](string wordStr, vector<string> word) {
    phoenix.learn(word,wordStr);
  };

  CtrlClaw claw;
  auto clawText = [&](string &path,string &content) {
    DUMP_VAR(path);
    //DUMP_VAR(content);
    claw.claw(content);
    claw.eachSentence(learnPhoenix);
    phoenix.commitArticle();
  };
  TextPump txtPump("./url_crawl/.wiki.ja.meta/");
  txtPump.eachNewText("phoenix.train.one_2",clawText);

  phoenix.unloadMaster();
  return 0;
}

