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
  ParrotWord parrot("./db/ostrich");
  parrot.loadMaster();
}
