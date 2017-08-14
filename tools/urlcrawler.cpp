#include <iostream>
#include <string>
#include <thread>
using namespace std;

void url_crawler_upd_main(void);
void url_crawler_collect(void);

int main(int ac, char *av[]) {
  std::thread t(url_crawler_upd_main);
  std::thread c(url_crawler_collect);
  t.join();
  c.join();
  return 0;
}
