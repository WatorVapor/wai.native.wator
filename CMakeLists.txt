cmake_minimum_required(VERSION 2.8)
project(charcut)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c14 -g -pthread ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -pthread ")

add_definitions(-DBOOST_SPIRIT_THREADSAFE)

include_directories(./)
include_directories(./watorword)
include_directories(./utilities)
include_directories(./tools)

set(LIBS
  boost_filesystem
  boost_system
  boost_thread
  boost_regex
  pthread
  leveldb
  ssl
  crypto
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
  watorword/treeword.cpp
  watorword/dictstorage.cpp
  watorword/ctrlclaw.cpp
  watorword/textpump.cpp
  watorword/wikicrawler.cpp
  watorword/taskpool_entry.cpp
  watorword/taskmerge_entry.cpp
  watorword/api_entry.cpp
  watorword/api_wai.cpp
  watorword/utf8.cpp
  #watorword/word_statistics_master.cpp
  watorword/word_db_master.cpp
  watorword/word_db_master2.cpp
  watorword/word_statistics_parrot.cpp
) 

add_executable(wator.crawler.app
  tools/crawler.cpp
)
target_link_libraries(wator.crawler.app
  watorword
  ${LIBS}
)

add_executable(wator.ostrich.app 
  tools/ostrich.cpp
)
target_link_libraries(wator.ostrich.app
  watorword
  ${LIBS}
)


add_executable(wator.parrot.app 
  tools/parrot.cpp
)
target_link_libraries(wator.parrot.app
  watorword
  ${LIBS}
)



add_executable(wator.phoenix.app 
  tools/phoenix.cpp
)
target_link_libraries(wator.phoenix.app
  watorword
  ${LIBS}
)




#add_executable(wator.display.app
#  tools/display.cpp
#)
#target_link_libraries(wator.display.app
#  watorword
#  ${LIBS}
#)

add_executable(wator.webservice.wai.app
  tools/wai.cpp
)
target_link_libraries(wator.webservice.wai.app
  watorword
  ${LIBS}
)


add_library(utilitieswator
  utilities/train_master_entry.cpp
  utilities/word_master_entry.cpp
)


add_executable(wator.webservice.taskpool.app
  tools/taskpool.cpp
)
target_link_libraries(wator.webservice.taskpool.app
  watorword
  ${LIBS}
)

add_executable(wator.webservice.taskmerge.app
  tools/taskmerge.cpp
)
target_link_libraries(wator.webservice.taskmerge.app
  watorword
  ${LIBS}
)

add_executable(wator.webservice.trainmaster.app
  tools/trainmaster.cpp
)
target_link_libraries(wator.webservice.trainmaster.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.webservice.wordmaster.app
  tools/wordmaster.cpp
)
target_link_libraries(wator.webservice.wordmaster.app
  utilitieswator
  ${LIBS}
)


