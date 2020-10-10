add_test_executable(NAME log_spd-test SOURCES log_spd-test.cc)
target_include_directories(log_spd-test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include>)
