#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#include "log.hpp"
#include "parrotword.hpp"

int main(int ac, char *av[]) {
  if(ac < 2) {
    return 0;
  }
  ParrotWord parrot(av[1]);
  parrot.loadMaster();
  return 0;
}
