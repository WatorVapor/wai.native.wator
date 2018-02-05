#include <iostream>
#include <string>
#include <thread>
using namespace std;

void api_upd_main(void);
void redis_main(void);
int main(int ac, char *av[]) {
  std::thread t1(api_upd_main);
  std::thread t2(redis_main);
  t1.join();
  t2.join();
  return 0;
}
