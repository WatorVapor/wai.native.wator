#include <iostream>
#include <string>
#include <thread>
using namespace std;

void train_ostrich_fetch_upd_main(void);
void train_ostrich_collect(void);
void train_ostrich_save_upd_main(void);

int main(int ac, char *av[]) {
  std::thread t(train_ostrich_fetch_upd_main);
  std::thread c(train_ostrich_collect);
  std::thread s(train_ostrich_save_upd_main);
  t.join();
  c.join();
  s.join();
  return 0;
}
