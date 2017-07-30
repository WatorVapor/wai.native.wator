#include "wikicrawler.hpp"

WikiCrawler::WikiCrawler() {}

WikiCrawler::~WikiCrawler() {}

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>


const static int iConstMaxConten = 1024*256;
const static string strMatchHref("href=\"");
void WikiCrawler::parse(const pt::ptree &task, string &content) {
  // DUMP_VAR(content);
  string filter;
  string prefix;
  string base_url;
  try {
    auto filterOpt = task.get_optional<string>("filter");
    if (filterOpt) {
      filter = filterOpt.get();
      DUMP_VAR(filter);
    }
    auto prefixOpt = task.get_optional<string>("prefix");
    if (prefixOpt) {
      prefix = prefixOpt.get();
      DUMP_VAR(prefix);
    }
    auto base_urlOpt = task.get_optional<string>("base_url");
    if (base_urlOpt) {
      base_url = base_urlOpt.get();
      DUMP_VAR(base_url);
    }
  } catch (const pt::json_parser::json_parser_error &e) {
    DUMP_VAR(e.what());
  } catch (const std::exception &ex) {
    DUMP_VAR(ex.what());
  }

  try {
    boost::regex rx("<a.*?href=['|\"](.*?)['|\"]");
    vector<string> hrefArrays;
    auto sliceLongTextCounter = content.size()/iConstMaxConten +1;
    
    //for(int i=0;i < sliceLongTextCounter ;i++) {
      //size_t length = content.size()%iConstMaxConten;
      //std::string slice = content.substr(i*iConstMaxConten,length);
      for (auto it = boost::sregex_iterator(content.begin(), content.end(), rx);
           it != boost::sregex_iterator(); ++it) {
        TRACE_VAR(it->position());
        boost::smatch match = *it;
        auto match_href = match.str();
        TRACE_VAR(match_href);
        auto first = match_href.find(strMatchHref);
        auto last = match_href.find_last_of("\"");
        if (first != std::string::npos && last != std::string::npos &&
            last > first) {
          string href = match_href.substr(first + strMatchHref.size(),
                                          last - first - strMatchHref.size());
          TRACE_VAR(href);
          auto first = href.find(filter);
          if (first == 0) {
            href = base_url + href;
            if (filter != prefix) {
              boost::algorithm::replace_all(href, filter, prefix);
            }
            hrefArrays.push_back(href);
            TRACE_VAR(filter, href);
          }
        }
      }
      string crawlerArrays;
      int counter = 1;
      for(auto href:hrefArrays) {  
        crawlerArrays += "{";
        crawlerArrays += href;
        crawlerArrays += "};";
        if(counter++ %iConstOnceUpURLMax == 0){
          this->up(task,crawlerArrays);
          crawlerArrays.clear();
        }
      }
      if(crawlerArrays.empty() == false) {
        this->up(task,crawlerArrays);
      }
    //}
  } catch (const std::exception &ex) {
    DUMP_VAR(ex.what());
  }
}

void WikiCrawler::up(const pt::ptree &task, string &urls) {
  pt::ptree upTask(task);
  upTask.put("crawler", urls);
  string task_url_upPath = "/tmp/wai.native/task_url_up.json";
  pt::write_json(task_url_upPath, upTask);
  string wgetTaskUp("curl -6 -F \"");
  // string wgetTaskUp("curl -F \"");
  wgetTaskUp += "file=@";
  wgetTaskUp += task_url_upPath;
  wgetTaskUp += "\" ";
  wgetTaskUp += "\"https://www.wator.xyz/wai/text/train/crawler\"";
  DUMP_VAR(wgetTaskUp);
  ::system(wgetTaskUp.c_str());
}
