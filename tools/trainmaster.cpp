#include <iostream>
#include <string>
#include <thread>
using namespace std;

void tain_master_upd_main(void);

int main(int ac, char *av[]) {
  std::thread t(tain_master_upd_main);
  t.join();
  return 0;
}
