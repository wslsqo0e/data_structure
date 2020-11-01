#include "base_utils.h"
#include "unicode_string.h"

std::string remove_comment(const std::string &input_str) {
  UnicodeString u_str = input_str.c_str();
  u_str.strip();
  if (u_str[0] == "#") {
    return "";
  } else {
    size_t pos = u_str.find(" #");
    if (pos != UnicodeString::npos) {
      UnicodeString remain = u_str.substr(0, pos);
      remain.strip();
      return remain.c_str();
    } else {
      return u_str.c_str();
    }
  }
}
