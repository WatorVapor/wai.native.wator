#include <iostream>
#include <string>
#include <thread>
using namespace std;

void taskpool_upd_main(void);
void taskpool_collect(void);

int main(int ac, char *av[]) {
  std::thread t(taskpool_upd_main);
  std::thread c(taskpool_collect);
  t.join();
  c.join();
  return 0;
}
