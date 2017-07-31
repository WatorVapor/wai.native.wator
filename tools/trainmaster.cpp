#include <iostream>
#include <string>
#include <thread>
using namespace std;

void tain_master_upd_main(void);
void tain_master_collect(void);

int main(int ac, char *av[]) {
  std::thread t(tain_master_upd_main);
  std::thread c(tain_master_collect);
  t.join();
  c.join();
  return 0;
}
