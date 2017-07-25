#include <string>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
namespace pt = boost::property_tree;
#include "log.hpp"



class TextPump {
public:
  TextPump();
  TextPump(const string &dir);
  TextPump(const string &url,const string &tag);
  ~TextPump();
  string statistics(void);
  // loop
  template<typename T> void eachTextFile(const string &dir,T fn){
    const fs::path path(dir);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        auto extension = p.extension().string();
        TRACE_VAR(extension);
        if(extension == ".txt"){
          string pathText= p.string();
          TRACE_VAR(pathText); 
          std::ifstream textStream(pathText);
          std::string str((std::istreambuf_iterator<char>(textStream)),
                         std::istreambuf_iterator<char>());
          fn(pathText,str);
          textStream.close();
        }
      }
    }
  }  
  template<typename T> void eachJsonFile(const string &dir,T fn){
    const fs::path path(dir);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        auto extension = p.extension().string();
        TRACE_VAR(extension);
        if(extension == ".json"){
          string pathText= p.string();
          fn(pathText);
        }
      }
    }
  }  
  template<typename T> void eachJsonFile(T fn){
    const fs::path path(dir_);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        auto extension = p.extension().string();
        TRACE_VAR(extension);
        if(extension == ".json"){
          string pathText= p.string();
          fn(pathText);
        }
      }
    }
  }
  template<typename T> void eachNewText(const string &tag,T fn){
    const fs::path path(dir_);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
      if (!fs::is_directory(p)){
        auto extension = p.extension().string();
        TRACE_VAR(extension);
        if(extension == ".json"){
          string pathText= p.string();
          parseMeta(pathText,tag,fn);
          iTototl++;
        }
      }
    }
  }  
  template<typename T> void eachTextFromMaster(T fn){
   DUMP_VAR2(url_,tag_);
    while(true) {
      pt::ptree task;
      string content;
      if(fetchMasterTask(task,content)) {
        fn(task,content);
      }
    }
  }

private:
  template<typename T> void parseMeta(const string &pathMeata,const string &tag,T fn){
      try {
        pt::ptree textMeta;
        pt::read_json(pathMeata, textMeta);
        string pathText;
        auto textPathOpt = textMeta.get_optional<string>("path");
        if(textPathOpt) {
          pathText = textPathOpt.get();
        }
        if(pathText.empty()==false){
          auto tagOpt = textMeta.get_optional<bool>(tag);
          if(tagOpt) {
            DUMP_VAR3(tagOpt.get(),pathMeata,pathText);
          } else {
            TRACE_VAR(pathMeata,pathText);
            std::ifstream textStream(prefix_ + "/" + pathText);
            TRACE_VAR(textStream.good());
            if(textStream.good()) {
              std::string str((std::istreambuf_iterator<char>(textStream)),
                             std::istreambuf_iterator<char>());
              fn(pathText,str);
              textStream.close();
              textMeta.put(tag,true);
              pt::write_json(pathMeata,textMeta);
              iProccessed++;
            }
          }
        }
      } catch (const pt::json_parser::json_parser_error& e) {
        DUMP_VAR(e.what());
      }
      catch( const std::exception & ex ) {
        DUMP_VAR(ex.what());
      }
  }
  bool fetchMasterTask(pt::ptree &task,string &content);
private:
  const string dir_;
  const string url_;
  const string tag_;
  const string prefix_ = "url_crawl";
  int iTototl = 0;
  int iProccessed = 0;
};
