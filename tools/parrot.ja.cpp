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
#include "parrotword.hpp"
#include "log.hpp"





int main(int ac,char*av[])
{
  CtrlClaw claw;
  ParrotWord parrot("./db/wiki.ja");
  if(parrot.loadMaster()==false) {
    return 0;
  }
  auto learnParrot = [&](string wordStr, vector<string> word) {
    parrot.learn(word,wordStr);
  };
  auto clawText = [&](string &path,string &content) {
    DUMP_VAR(path);
    //DUMP_VAR(content);
    claw.claw(content);
    claw.eachSentence(learnParrot);
    parrot.commitArticle();
  };
  TextPump txtPump("url_crawl/.wiki.ja.meta/");
  txtPump.eachNewText("parrot.train.one",clawText);
  parrot.unloadMaster();
  return 0;
}
