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
  watorword/phoenixword.graph.cpp
  watorword/qiangbaoword.cpp
  watorword/qiangbaoword.db.cpp
  watorword/treeword.cpp
  #watorword/dictmaster.cpp
  watorword/ctrlclaw.cpp
  watorword/textpump.cpp
  watorword/wikicrawler.cpp
  watorword/api_entry.cpp
  watorword/api_wai.cpp
  watorword/utf8.cpp
  #watorword/word_statistics_master.cpp
  watorword/word_db_master.cpp
  watorword/word_db_master2.cpp
  #watorword/word_statistics_parrot.cpp
  watorword/log.cpp
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
  utilities/service_ostrich_entry.cpp
  utilities/service_ostrich_fetch_task.cpp
  utilities/service_ostrich_save_task.cpp
  utilities/service_parrot_entry.cpp
  utilities/service_parrot_fetch_task.cpp
  utilities/service_parrot_save_task.cpp
  utilities/service_phoenix_entry.cpp
  utilities/service_phoenix_fetch_task.cpp
  utilities/service_phoenix_save_task.cpp
#  utilities/url_pool_entry.cpp
#  utilities/url_merge_entry.cpp
  utilities/url_crawler_entry.cpp
  utilities/url_crawler_fetch_task.cpp
  utilities/url_crawler_save_task.cpp
  utilities/urlstorage.cpp
  utilities/dictstorage.cpp
  utilities/trainstorage.cpp
  utilities/udp_entry.cpp
  utilities/log.cpp
)


add_executable(wator.webservice.crawler.app
  tools/service_crawler.cpp
)
target_link_libraries(wator.webservice.crawler.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.webservice.ostrich.app
  tools/service_ostrich.cpp
)
target_link_libraries(wator.webservice.ostrich.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.webservice.parrot.app
  tools/service_parrot.cpp
)
target_link_libraries(wator.webservice.parrot.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.webservice.phoenix.app
  tools/service_phoenix.cpp
)
target_link_libraries(wator.webservice.phoenix.app
  utilitieswator
  ${LIBS}
)


add_executable(wator.collect.app
  tools/collect.cpp
)
target_link_libraries(wator.collect.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.joint.app
  tools/joint.cpp
)
target_link_libraries(wator.joint.app
  utilitieswator
  ${LIBS}
)

add_executable(wator.castdb.app 
  tools/castdb.cpp
)
target_link_libraries(wator.castdb.app
  watorword
  ${LIBS}
)

