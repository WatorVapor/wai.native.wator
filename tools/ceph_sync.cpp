#include <iostream>
#include <string>
#include <thread>
using namespace std;

#include "log.hpp"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

int main(int ac, char *av[]) {
  if(ac <3) {
    DUMP_VAR(ac);
    return 0;
  }
  try {
    fs::path path(av[1]);
    BOOST_FOREACH (const fs::path &p,
                   std::make_pair(fs::recursive_directory_iterator(path),
                                  fs::recursive_directory_iterator())) {
      if (!fs::is_directory(p)) {
        string pathText = p.string();
        DUMP_VAR(pathText);
        string cmd = "cp -f ";
        cmd += pathText;
        cmd += " ";
        cmd += av[2];
        cmd += "/";
      }
    }
  } catch (std::exception &e) {
    DUMP_VAR(e.what());
  } catch (...) {
  }
  return 0;
}
