#include <iostream>
#include <string>
#include <thread>
using namespace std;

void tain_fetch_upd_main(void);

int main(int ac, char *av[]) {
  std::thread t(tain_fetch_upd_main);
  t.join();
  return 0;
}
