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
#include "parrotword.hpp"
#include "textpump.hpp"

vector<string> parseUTF8(const string &words);

void parseBytes(const vector<string> &Bytes, const string &text);

static string longTxt(
    u8"它是由从地方到全球范围内几百万个私人的、学术界的、"
    u8"企业的和政府的网络所构成，通过电子，"
    u8"企业的和政府的网络所构成，通过<>"
    u8"无线和光纤网络技术等等一系列广泛的技术联系在一起。"
    u8"这种将计算机网络互相联接在一起的方法可称作“网络互联”，"
    u8"在这基础上发展出覆盖全世界的全球性互联网络称互联网，"
    u8"即是互相连接一起的网络。");

static string shortTxt(
    u8"人造卫星能够成功执行预定任务，单凭卫星本身是不行的，"
    u8"而需要完整的卫星工程系统");

int main(int ac, char *av[]) {
  ParrotWord parrot("./db/ostrich/cn");
  if (parrot.loadMaster(false) == false) {
    return 0;
  }
  auto learnParrot = [&](string wordStr, vector<string> word) {
    parrot.learn(word, wordStr);
  };

  CtrlClaw claw;
  claw.claw(longTxt);
  claw.eachSentence(learnParrot);

  parrot.unloadMaster();
  return 0;
}

/*
int main(int ac, char *av[]) {
  CtrlClaw claw;
  ParrotWord parrot("./db/baidu.baike");
  if (parrot.loadMaster() == false) {
    return 0;
  }
  auto learnParrot = [&](string wordStr, vector<string> word) {
    parrot.learn(word, wordStr);
  };
  auto clawText = [&](string &path, string &content) {
    DUMP_VAR(path);
    // DUMP_VAR(content);
    claw.claw(content);
    claw.eachSentence(learnParrot);
    parrot.commitArticle();
  };
  TextPump txtPump("url_crawl/.baike.baidu.meta/");
  txtPump.eachNewText("parrot.train.one_1", clawText);
  parrot.unloadMaster();
  return 0;
}
*/
