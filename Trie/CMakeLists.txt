add_test_executable(NAME darts-test SOURCES darts-test.cc DEPENDS log_base)
target_include_directories(darts-test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
