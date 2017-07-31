#include <iostream>
#include <string>
#include <thread>
using namespace std;

void urlpool_upd_main(void);
void urlpool_collect(void);

int main(int ac, char *av[]) {
  std::thread t(urlpool_upd_main);
  std::thread c(urlpool_collect);
  t.join();
  c.join();
  return 0;
}
