#include <iostream>
#include <string>
#include <thread>
using namespace std;

void train_fetch_upd_main(void);
void train_collect(void);
void train_save_upd_main(void);
void train_word_write(void);

int main(int ac, char *av[]) {
  std::thread t(tain_fetch_upd_main);
  std::thread c(train_collect);
  std::thread s(train_save_upd_main);
  std::thread w(train_word_write);
  t.join();
  c.join();
  s.join();
  w.join();
  return 0;
}
