
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <condition_variable>
#include <mutex>

static vector<string> gVectTodoPathCN;
static vector<string> gVectTodoPathJA;
static std::mutex gVectoPathMutex;
static std::condition_variable gVectoPathCV;
static std::mutex gVectoPathCvMutex;

static int iConstPathCacheMax = 32;

string fetchOstrichTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
  return "";
}
string fetchParrotTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
  return "";
}
string fetchPhoenixTask(const string &lang) {
  DUMP_VAR(lang);
  if (lang == "cn") {
  } else if (lang == "ja") {
  } else {
  }
  return "";
}
