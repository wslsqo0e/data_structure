#pragma once

#include <stdint.h>

class UnicodeChar {
public:
  UnicodeChar();
  UnicodeChar(const char* c);
  UnicodeChar(const int& c);
  UnicodeChar(const UnicodeChar& other);
  UnicodeChar& operator = (const UnicodeChar& other);
  UnicodeChar& operator = (const char* c);
  UnicodeChar& operator = (const int& c);
  ~UnicodeChar();
  int set_char(const char* src);
  int set_char(char* src, char* end);
  uint32_t convert_to_utf32();
  const char* c_str() const;

  unsigned int get_code() const;

  bool is_null() const;
  bool is_null();

  bool operator == (const UnicodeChar& other) const;
  bool operator != (const UnicodeChar& other) const;
  bool operator < (const UnicodeChar& other) const;
  //uint16_t convertToUTF16();
private:
  bool is_legal_utf8_char(const char *c) const;
  bool is_legal_utf8_char() const;
  char _cc[5];    //Unicode 最长4个字节

};

bool operator<(const char& a, const UnicodeChar& b);
bool operator<(const UnicodeChar& a, const char& b);
bool operator>(const char& a, const UnicodeChar& b);
bool operator>(const UnicodeChar& a, const char& b);
bool operator==(const char& a, const UnicodeChar& b);
bool operator==(const UnicodeChar& a, const char& b);
bool operator!=(const char& a, const UnicodeChar& b);
bool operator!=(const UnicodeChar& a, const char& b);
UnicodeChar operator+(const UnicodeChar& a, const int& b);
UnicodeChar operator+(const int& a, const UnicodeChar& b);
UnicodeChar operator-(const UnicodeChar& a, const int& b);
UnicodeChar operator-(const int& a, const UnicodeChar& b);
bool operator<=(const char& a, const UnicodeChar& b);
bool operator<=(const UnicodeChar& a, const char& b);
bool operator>=(const char& a, const UnicodeChar& b);
bool operator>=(const UnicodeChar& a, const char& b);

bool operator<=(const int& a, const UnicodeChar& b);
bool operator<=(const UnicodeChar& a, const int& b);
bool operator>=(const int& a, const UnicodeChar& b);
bool operator>=(const UnicodeChar& a, const int& b);
