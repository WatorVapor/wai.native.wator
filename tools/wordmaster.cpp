#include <iostream>
#include <string>
#include <thread>
using namespace std;

void wordmaster_upd_main(void);
void wordmaster_write_main(void);
int main(int ac, char *av[]) {
  std::thread t(wordmaster_upd_main);
  std::thread w(wordmaster_write_main);
  t.join();
  w.join();
  return 0;
}
