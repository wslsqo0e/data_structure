cmake_minimum_required (VERSION 3.1)
project (utilcxx)

# 使得 Make 时打印更多内容
# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")

#include_directories(${PROJECT_SOURCE_DIR}/src)
#include_directories(../../basic/spdlog/include/ ../../basic/inih/ ../../basic/json/include/ ../../basic/darts/ ../../basic/common)

aux_source_directory(${PROJECT_SOURCE_DIR} DIRSRCS)
link_libraries(-liconv)

add_executable(main_test ${DIRSRCS})

#set(NGRAM_SRCS ${DIRSRCS})
#list(APPEND NGRAM_SRCS ${PROJECT_SOURCE_DIR}/bin_src/ngram.cpp)
#add_executable(ngram ${NGRAM_SRCS})
