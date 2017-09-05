#include <iostream>
#include <string>
#include <thread>
using namespace std;

void train_parrot_fetch_upd_main(void);
void train_parrot_collect(void);
void train_parrot_save_upd_main(void);
void train_parrot_word_write(void);

int main(int ac, char *av[]) {
  std::thread t(train_parrot_fetch_upd_main);
  std::thread c(train_parrot_collect);
  std::thread s(train_parrot_save_upd_main);
  std::thread w(train_parrot_word_write);
  t.join();
  c.join();
  s.join();
  w.join();
  return 0;
}
