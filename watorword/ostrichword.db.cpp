#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <map>
using namespace std;

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <boost/format.hpp>

#include "ostrichword.hpp"

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define TRACE_VAR(x)
