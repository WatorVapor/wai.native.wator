#include <iostream>
#include <string>
#include <thread>
using namespace std;

bool loadMasterDB(void);
void unloadMasterDB(void);

//void api_upd_main(void);
void redis_sub_main(void);
void redis_pub_main(void);
int main(int ac, char *av[]) {
  //std::thread t1(api_upd_main);
  loadMasterDB();
  std::thread t2(redis_sub_main);
  std::thread t3(redis_pub_main);
  //t1.join();
  t2.join();
  t3.join();
  unloadMasterDB();
  return 0;
}
