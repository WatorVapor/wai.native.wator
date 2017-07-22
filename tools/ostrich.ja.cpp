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





int main(int ac,char*av[])
{
  OstrichWord ostrich("./db/wiki.ja");
  auto learnOstrich = [&](const vector<string> &word) {
    ostrich.learn(word);
  };
  CtrlClaw claw;  
  auto clawText = [&](string &path,string &content) {
    DUMP_VAR(path);
    //DUMP_VAR(content);
    claw.claw(content);
    claw.eachMultiByte(learnOstrich);
    ostrich.mergeWordByArticle();
  };
  TextPump txtPump("url_crawl/.wiki.ja.meta/");
/*  
  auto parseJson = [&](string &path) {
    DUMP_VAR(path);
  };
  txtPump.eachJsonFile(parseJson);
  auto showText = [&](string &path,string &content) {
    DUMP_VAR(path);
    //DUMP_VAR(content);
  };
*/  
  txtPump.eachNewText("ostrich.train.one",clawText);
  return 0;
}

