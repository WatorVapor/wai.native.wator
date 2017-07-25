#pragma once
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#if 0
#define DUMP_VAR(x)                                                            \
  {                                                                            \
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << "::" << #x << "=<" << x        \
                             << ">";                                           \
  }
#endif

#define DUMP_VAR(x) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "=<" << x << ">"<< std::endl;
#define DUMP_VAR2(x,y) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "=<" << x << "," << y << ">" << std::endl;
#define DUMP_VAR3(x,y,z) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "=<" << x << "," << y << "," << z <<">" << std::endl;
#define DUMP_VAR4(x,y,z,a) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "=<" << x << "," << y << "," << z << "," << a <<">" << std::endl;
#define DUMP_VAR5(x,y,z,a,b) std::cout << __func__ << ":" << __LINE__ << "::" << #x << "," << #y << "," << #z << "," << #a << "," << #b << "=<" << x << "," << y << "," << z << "," << a << "," << b <<">" << std::endl;

#define TRACE_VAR(...)
