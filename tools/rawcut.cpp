#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <streambuf>
using namespace std;
#include "charcut.hpp"

#include "log.hpp"


vector<string> parseUTF8(const string & words);
vector<char32_t> utf8toCode(const string & words);

string readTextWiki(const string &title);
string readTextBaidu(const string &title);
void crawBaike(CharCut & cut);
void crawLocalText(CharCut & cut);



static vector<string> gTitleList = {
  u8"日本",
  u8"日本語",u8"中国",u8"東京",
  u8"困難",u8"国文",u8"琉球",
  u8"世界",u8"方言",u8"研究",
  u8"動詞",u8"仮名",u8"関係",
  u8"言葉",u8"現在",u8"江戸",
  u8"文字",u8"京都",u8"経済",
  u8"朝鮮",u8"形容",u8"調査",
  u8"国家",u8"人口",u8"活用",
  u8"琉球",u8"英語",u8"条約",
  u8""
};

static vector<string> gTitleBaiduList = {
 /*
  u8"蜗鸢",u8"湖泊",u8"灌木",u8"浙江",
  u8"江苏",u8"山东",u8"上海",u8"安徽",
  u8"广东",u8"江西",u8"福建",u8"福建",
  u8"广西",u8"海南",u8"河南",u8"湖南",
  u8"宁夏",u8"河北",u8"山西",u8"天津",
  u8"青海",u8"陕西",u8"甘肃",u8"新疆",
  u8""
 */
};

void api_upd_main(void);

int main(int ac,char*av[])
{
  std::thread udp(api_upd_main);
  udp.detach();
  CharCut cutter;
#if 1
  for(const auto &title:gTitleBaiduList){
    string text = readTextBaidu(title);
    auto input = parseUTF8(text);
    cutter.pushContent(input);
  }
  crawBaike(cutter);
  crawLocalText(cutter);
#endif
#if 0
  for(const auto &title:gTitleList){
    string text = readTextWiki(title);
    auto input = parseUTF8(text);
    cutter.pushContent(input);
  }
#endif
//  cutter.compressRanking();
  cutter.dumpSum();
  return 0;
}

string readTextWiki(const string &title) {
  string cmd = "wget --no-check-certificate \"https://ja.wikipedia.org/w/api.php?format=xml&action=query&prop=revisions&rvprop=content";
  cmd += "&titles=";
  cmd += title;
  cmd += "\" -O wget.text.word";
  ::system(cmd.c_str());
  std::ifstream t("wget.text.word");
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return str;
}


string readTextBaidu(const string &title) {
  string cmd = "wget --no-check-certificate \"http://baike.baidu.com/item/";
  cmd += title;
  cmd += "\" -O /tmp/wget.text.word";
  ::system(cmd.c_str());
  std::ifstream t("/tmp/wget.text.word");
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return str;
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;

void crawBaike(CharCut & cutter){
  string crawl = "./url_crawl/out.baike.baidu.url.json";
  pt::ptree config;
  try {
    pt::read_json(crawl, config);
    BOOST_FOREACH (const pt::ptree::value_type& child, config.get_child("urls")) {
      auto key = child.first;
      TRACE_VAR(key);
      auto textObj = child.second;
      auto pathOpt = textObj.get_optional<std::string>("path");
      if(pathOpt) {
        auto pathText = pathOpt.get();
        if(pathText.empty()==false) {
          DUMP_VAR(pathText);
          string path= "url_crawl/" + pathText;
          DUMP_VAR(path);
          std::ifstream t(path);
          std::string str((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());
          auto input = parseUTF8(str);
          cutter.pushContent(input);
        }
      }
    }
  } catch(...) {
  }
}

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;
void crawLocalText(CharCut & cutter){
  const fs::path path("url_crawl/textout");
  BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
    if (!fs::is_directory(p)){
      auto extension = p.extension().string();
      DUMP_VAR(extension);
      if(extension == ".txt"){
        string path= p.string();
        DUMP_VAR(path);
        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());
        auto input = parseUTF8(str);
        cutter.pushContent(input);
      }
    }
  }
}
