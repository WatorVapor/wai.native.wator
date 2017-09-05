#include <iostream>
#include <string>
#include <thread>
using namespace std;

void url_crawler_fetch_upd_main(void);
void url_crawler_collect(void);
void url_crawler_save_upd_main(void);
void url_crawler_write_main(void);

int main(int ac, char *av[]) {
  std::thread t(url_crawler_fetch_upd_main);
  std::thread c(url_crawler_collect);
  std::thread s(url_crawler_save_upd_main);
  std::thread w(url_crawler_write_main);
  t.join();
  c.join();
  s.join();
  w.join();
  return 0;
}
