#include <algorithm>
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

std::string Q2B(const std::string &input) {
  std::string output;
  UnicodeString u_str = input.c_str();
  for (int i = 0; i < u_str.get_len(); i++) {
    UnicodeChar cc = u_str[i];
    unsigned code = cc.get_code();
    if (code == 12288) {
      cc = 32;
    } else if (code >= 65281 && code <= 65374) {
      code -= 65248;
      cc = code;
    }
    output.append(cc.c_str());
  }
  return output;
}

std::string B2Q(const std::string &input) {
  std::string output;
  UnicodeString u_str = input.c_str();
  for (int i = 0; i < u_str.get_len(); i++) {
    UnicodeChar cc = u_str[i];
    unsigned code = cc.get_code();
    if (code == 32) {
      cc = 12288;
    } else if (code >= 33 && code <= 126) {
      code += 65248;
      cc = code;
    }
    output.append(cc.c_str());
  }
  return output;
}

std::vector<std::string> string_split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;
  while((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
    token = s.substr (pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }
  res.push_back(s.substr(pos_start));
  return res;
}
