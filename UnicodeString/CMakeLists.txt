set(UNICODE_STRING_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ PARENT_SCOPE)
set(UNICODE_STRING_LIB_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/unicode_char.cpp;${CMAKE_CURRENT_SOURCE_DIR}/unicode_string.cpp;"
  PARENT_SCOPE)
add_library(unicodestring unicode_char.cpp unicode_string.cpp)
