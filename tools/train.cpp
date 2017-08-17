#include <iostream>
#include <string>
#include <thread>
using namespace std;

void train_fetch_upd_main(void);
void train_collect(void);
void train_save_upd_main(void);
void train_save(void);

int main(int ac, char *av[]) {
  std::thread t(tain_fetch_upd_main);
  std::thread c(train_collect);
  t.join();
  c.join();
  return 0;
}
