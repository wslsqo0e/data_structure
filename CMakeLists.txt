cmake_minimum_required (VERSION 3.1)
project (data_structure)

# define macro
macro(add_test_executable)
  cmake_parse_arguments(test_exe "" "NAME" "SOURCES;DEPENDS" ${ARGN})
  add_executable(${test_exe_NAME} ${test_exe_SOURCES})
  target_link_libraries(${test_exe_NAME} PRIVATE ${test_exe_DEPENDS})
  add_test(
    NAME ${test_exe_NAME}
    COMMAND ${test_exe_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  set_target_properties(${test_exe_NAME}  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/testbin)
  install(TARGETS ${test_exe_NAME} RUNTIME DESTINATION testbin)

  unset(test_exe_NAME)
  unset(test_exe_SOURCES)
  unset(test_exe_DEPENDS)
endmacro()
# end define macro

# 使得 Make 时打印更多内容
# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)

# 使得bin文件进入 bin/ 文件夹下
# 也可以通过 set_target_properties 对单个targe进行设置
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_STANDARD 14)

include(CTest)
enable_testing()

add_subdirectory(Logger)
include_directories(${LOGGER_INCLUDE_PATH})
add_subdirectory(Trie)
add_subdirectory(UnicodeString)
include_directories(${UNICODE_STRING_INCLUDE_PATH})
add_subdirectory(base_utils)

add_library(data_structure ${LOGGER_LIB_PATH}
  ${BASE_UTILS_LIB_PATH}
  ${UNICODE_STRING_LIB_PATH})

# for output
set(DATA_STRUCTURE_INCLUDE_PATH "${LOGGER_INCLUDE_PATH};${CMAKE_CURRENT_SOURCE_DIR};${BASE_UTILS_INCLUDE_PATH};${UNICODE_STRING_INCLUDE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/utilcxx" PARENT_SCOPE)
