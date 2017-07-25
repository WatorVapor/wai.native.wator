#include "wikicrawler.hpp"

WikiCrawler::WikiCrawler(){
}

WikiCrawler::~WikiCrawler(){
}

#include <regex>

const static string strMatchHref("href=\"");
void WikiCrawler::parse(const pt::ptree &task,string &content) {
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
  wgetTaskUp += "\"https://www.wator.xyz/wai/text/train/crawler\"";
  DUMP_VAR(wgetTaskUp);
}
