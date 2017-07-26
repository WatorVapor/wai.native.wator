#include <string>
#include <iostream>
#include <thread>
using namespace std;

void taskpool_upd_main(void);
int main(int ac,char*av[])
{
  std::thread t(api_upd_main);
  t.join();
  return 0;
}
