set(BASE_UTILS_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ PARENT_SCOPE)
set(BASE_UTILS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/base_utils.cpp" PARENT_SCOPE)
add_library(base_utils base_utils.cpp)

add_test_executable(NAME base_utils-test SOURCES base_utils-test.cc DEPENDS base_utils log_base unicodestring)
target_include_directories(base_utils-test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
