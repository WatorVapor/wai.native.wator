#include <iostream>
#include <string>
#include <thread>
using namespace std;

void urlmerge_upd_main(void);
void urlmerge_write_main(void);
int main(int ac, char *av[]) {
  std::thread t(taskmerge_upd_main);
  std::thread w(taskmerge_write_main);
  t.join();
  w.join();
  return 0;
}
