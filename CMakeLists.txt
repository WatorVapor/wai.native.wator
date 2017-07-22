cmake_minimum_required(VERSION 2.8)
project(charcut)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c14 -g -pthread ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -pthread ")

add_definitions(-DBOOST_SPIRIT_THREADSAFE)

include_directories(./)
include_directories(./watorword)
include_directories(./tools)

set(LIBS
  boost_filesystem
  boost_system
  boost_thread
  pthread
  leveldb
)
add_library(watorword
  watorword/ostrichword.cpp
  watorword/ostrichword.db.cpp
  watorword/parrotword.cpp
  watorword/parrotword.db.cpp
  watorword/phoenixword.cpp
  watorword/phoenixword.db.cpp
  watorword/qiangbaoword.cpp
  watorword/qiangbaoword.db.cpp
  watorword/dictstorage.cpp
  watorword/ctrlclaw.cpp
  watorword/textpump.cpp
  watorword/treeword.cpp
  watorword/api_entry.cpp
  watorword/api_wai.cpp
  watorword/utf8.cpp
  #watorword/word_statistics_master.cpp
  watorword/word_db_master.cpp
  watorword/word_db_master2.cpp
  watorword/word_statistics_parrot.cpp
) 


add_executable(wator.ostrich.app 
  ostrich.cpp
)
target_link_libraries(wator.ostrich.app
  watorword
  ${LIBS}
)


add_executable(wator.parrot.app 
  parrot.cpp
)
target_link_libraries(wator.parrot.app
  watorword
  ${LIBS}
)



add_executable(wator.phoenix.app 
  phoenix.cpp
)
target_link_libraries(wator.phoenix.app
  watorword
  ${LIBS}
)




add_executable(wator.display.app display.cpp)
target_link_libraries(wator.display.app
  watorword
  ${LIBS}
)

add_executable(wator.wai.app wai.cpp)
target_link_libraries(wator.wai.app
  watorword
  ${LIBS}
)

