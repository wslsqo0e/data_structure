#pragma once

#include <vector>
#include <stdlib.h>
#include <string>
#include <stdexcept>
#include "unicode_char.h"

class UnicodeStringReallocateError : public std::exception {
public:
    UnicodeStringReallocateError(const std::string message = "") : _message(message) {}

    virtual const char* waht() const throw() {
        return _message.c_str();
    }
    virtual ~UnicodeStringReallocateError() throw() {}
private:
    std::string _message;
};

const size_t _SPAD_SIZE = 64;

class UnicodeString {
public:
    UnicodeString(const char* src = NULL);  // 默认传进来的就是 utf8 字符串
    UnicodeString(const UnicodeString& other);
    UnicodeString& operator = (const UnicodeString& other);
    UnicodeString& operator = (const char* src);
    ~UnicodeString();
    size_t get_len() const;
    size_t get_byte_len() const;
    const char* c_str() const;
    UnicodeChar operator[](int pos) const;

    bool is_null() const;
    bool empty() const;
    void clear();

    std::vector<UnicodeChar> split_char() const;
    std::vector<UnicodeString> split_char_s();
    std::vector<UnicodeString> split(UnicodeString sep = "", int maxsplit = -1);
    UnicodeString join(std::vector<UnicodeString>& lists);
    UnicodeString join(std::vector<std::string>& lists);
    void strip(UnicodeString sp = "");
    void rstrip(UnicodeString sp = "");
    void lstrip(UnicodeString sp = "");
    UnicodeString substr(size_t pos, size_t len = npos);

    UnicodeString operator+(const UnicodeChar& other) const;
    UnicodeString operator+(const UnicodeString& other) const;
    UnicodeString operator+(const char* other) const;
    UnicodeString& operator+=(const UnicodeChar& other);
    UnicodeString& operator+=(const UnicodeString& other);
    UnicodeString& operator+=(const char* other);
    bool operator ==(const UnicodeString& other) const;
    bool operator !=(const UnicodeString& other) const;
    bool operator <(const UnicodeString& other) const;

    size_t find(const UnicodeString& str, size_t pos = 0);
    size_t find(const char* s, size_t pos = 0);
    UnicodeString& replace(size_t pos, size_t len, const UnicodeString& str);
    UnicodeString& replace(size_t pos, size_t len, const char* s);
    UnicodeString& replace(const char* src, const char* dest, int count = 0);
    UnicodeString& replace(UnicodeString& src, UnicodeString& dest, int count = 0);

    UnicodeString lower();
    UnicodeString upper();
    bool is_digit();

    const static size_t npos = -1;
private:
    mutable char _spad[_SPAD_SIZE];
    char* _p_ref;     // 一旦 _p_ref != NULL, 则字符串必然存于 _p_ref
    size_t _capacity;   //记录_p_ref分配内存的长度

    size_t _byte_len;   // 字节长度
    mutable size_t _char_len;   // 字符长度

    mutable bool _flag_char_len;    // 字符长度标记
    bool _flag_is_null;     // 是否为空字符串

    mutable size_t _cursor_pos;        // 游标，提速迭代
    mutable char* _cursor_point;

    // 重新分配内存
    bool re_allocate(unsigned int len);
    void assign(const char* src);

    UnicodeString(const char* begin, const char* end);
    size_t cal_len() const;
    //isLegalUTF8()
};

UnicodeString operator+(const UnicodeChar& a, const UnicodeChar& b);
UnicodeString operator+(const char* a, const UnicodeString& b);
bool operator==(const char* a, const UnicodeString& b);
bool operator!=(const char* a, const UnicodeString& b);
