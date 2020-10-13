add_test_executable(NAME log_spd-test SOURCES log_spd-test.cc)
target_include_directories(log_spd-test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include>)

set(LOGGER_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include PARENT_SCOPE)

add_library(log-base log_base.cc)
target_include_directories(log-base PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include>)
