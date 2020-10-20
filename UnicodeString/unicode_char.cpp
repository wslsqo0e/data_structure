#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "unicode_char.h"

extern char g_utf8_bytes[256];
extern char g_first_byte_mark[7];
extern uint32_t g_utf8_offsets[6];

const uint32_t REPLACEMENT = 0x0000FFFD;

UnicodeChar::UnicodeChar() {
    bzero(_cc, 5);
}

UnicodeChar::UnicodeChar(const char* c) {
    bzero(_cc, 5);
    set_char(c);
}

UnicodeChar::UnicodeChar(const int& c) {
    bzero(_cc, 5);
    if (c >= 0 && c <= 0x7F) {
        _cc[0] = c;
    } else if (c > 0x7F && c <= 0x7FF) {
        _cc[0] = 0b11000000 + (c>>6);
        _cc[1] = 0b10000000 + (c&0x3F);
    } else if (c > 0x7FF && c <= 0xFFFF) {
        _cc[0] = 0b11100000 + (c>>12);
        _cc[1] = 0b10000000 + ((c>>6)&0x3F);
        _cc[2] = 0b10000000 + (c&0x3F);
    } else if (c > 0xFFFF && c <= 0x10FFFF) {
        _cc[0] = 0b11110000 + (c>>18);
        _cc[1] = 0b10000000 + ((c>>12)&0x3F);
        _cc[2] = 0b10000000 + ((c>>6)&0x3F);
        _cc[3] = 0b10000000 + (c&0x3F);
    } else {
        // do nothing
    }
}

UnicodeChar::UnicodeChar(const UnicodeChar& other) {
    memcpy(_cc, other._cc, 5);
}

UnicodeChar& UnicodeChar::operator = (const UnicodeChar& other) {
    memcpy(_cc, other._cc, 5);
    return *this;
}

UnicodeChar& UnicodeChar::operator = (const char* c) {
    set_char(c);
    return *this;
}

UnicodeChar& UnicodeChar::operator = (const int& c) {
    bzero(_cc, 5);
    if (c >= 0 && c <= 0x7F) {
        _cc[0] = c;
    } else if (c > 0x7F && c <= 0x7FF) {
        _cc[0] = 0b11000000 + (c>>6);
        _cc[1] = 0b10000000 + (c&0x3F);
    } else if (c > 0x7FF && c <= 0xFFFF) {
        _cc[0] = 0b11100000 + (c>>12);
        _cc[1] = 0b10000000 + ((c>>6)&0x3F);
        _cc[2] = 0b10000000 + (c&0x3F);
    } else if (c > 0xFFFF && c <= 0x10FFFF) {
        _cc[0] = 0b11110000 + (c>>18);
        _cc[1] = 0b10000000 + ((c>>12)&0x3F);
        _cc[2] = 0b10000000 + ((c>>6)&0x3F);
        _cc[3] = 0b10000000 + (c&0x3F);
    } else {
        // do nothing
    }
    return *this;
}

UnicodeChar::~UnicodeChar() {
    bzero(_cc, 5);
}

int UnicodeChar::set_char(const char* src) {
    if (!is_legal_utf8_char(src)) {
        return 0;
    }

    int src_len = strlen(src);

    if (src_len <= 0 || src_len > 4) {
        return 0;
    }

    memcpy(_cc, src, src_len);
    _cc[src_len + 1] = 0;
    return src_len;
}

int UnicodeChar::set_char(char* src, char* end) {
    int src_len = end - src;

    if (src_len <= 0 || src_len > 4) {
        return 0;
    }

    memcpy(_cc, src, src_len);
    _cc[src_len + 1] = 0;
    return 0;
}

uint32_t UnicodeChar::convert_to_utf32() {
    uint32_t c = 0;
    char* begin = _cc;
    uint8_t bb = *begin;
    uint8_t bytes = g_utf8_bytes[bb];

    switch (bytes) {
    case 5:         // illegal utf8
        c = REPLACEMENT;
        c = c << 6;

    case 4:         // illegal utf8
        c = REPLACEMENT;
        c = c << 6;

    case 3:
        c += *begin++;
        c = c << 6;

    case 2:
        c += *begin++;
        c = c << 6;

    case 1:
        c += *begin++;
        c = c << 6;

    case 0:
        c += *begin++;
    }

    c -= g_utf8_offsets[bytes];
    return c;
}

const char* UnicodeChar::c_str() const {
    return _cc;
}

unsigned int UnicodeChar::get_code() const {
    if (!is_legal_utf8_char()) {
        return 0;
    }

    uint8_t bb = *_cc;
    uint8_t bytes = g_utf8_bytes[bb];

    unsigned int ret = 0;

    switch (bytes) {
    case 5:
    case 4:
        return 0;

    case 3:
        ret += (_cc[0]&7) << 18;
        ret += (_cc[1]&0x3F) << 12;
        ret += (_cc[2]&0x3F) << 6;
        ret += _cc[3]&0x3F;
        break;
    case 2:
        ret += (_cc[0]&0xF) << 12;
        ret += (_cc[1]&0x3F) << 6;
        ret += _cc[2]&0x3F;
        break;
    case 1:
        ret += (_cc[0]&0x1F) << 6;
        ret += _cc[1]&0x3F;
        break;
    case 0:
        ret += _cc[0]&0x7F;
        break;
    default:
        break;
    }

    return ret;
}

bool UnicodeChar::is_legal_utf8_char(const char *c) const {
    if (c == NULL) {
        return false;
    }
    int len_cc = strlen(c);

    if (len_cc > 4) {
        return false;
    }

    uint8_t bb = *c;
    uint8_t bytes = g_utf8_bytes[bb];

    switch (bytes) {
    case 5:
    case 4:
        return false;

    case 3:
    case 2:
    case 1:
    case 0:
        if (len_cc != bytes + 1) {
            return false;
        }

        break;

    default:
        return false;
    }

    for (int i = 1; i < len_cc; i++) {
        unsigned char tt = *(c + i);
        tt = tt >> 6;

        if (tt != 2) {
            return false;
        }
    }

    return true;
}

bool UnicodeChar::is_legal_utf8_char() const {
    int len_cc = strlen(_cc);

    if (len_cc > 4) {
        return false;
    }

    uint8_t bb = *_cc;
    uint8_t bytes = g_utf8_bytes[bb];

    switch (bytes) {
    case 5:
    case 4:
        return false;

    case 3:
    case 2:
    case 1:
    case 0:
        if (len_cc != bytes + 1) {
            return false;
        }

        break;

    default:
        return false;
    }

    for (int i = 1; i < len_cc; i++) {
        unsigned char tt = *(_cc + i);
        tt = tt >> 6;

        if (tt != 2) {
            return false;
        }
    }

    return true;
}

bool UnicodeChar::is_null() const {
    if (*_cc) {
        return false;
    }

    return true;
}

bool UnicodeChar::is_null() {
    if (*_cc) {
        return false;
    }

    return true;
}

bool UnicodeChar::operator == (const UnicodeChar& other) const {
    if (strcmp(_cc, other._cc) == 0) {
        return true;
    }

    return false;
}

bool UnicodeChar::operator < (const UnicodeChar& other) const {
    if (strcmp(_cc, other._cc) < 0) {
        return true;
    }

    return false;
}

//uint16_t UnicodeChar::convertToUTF16()
//{
//
//}

bool operator<(const char&a, const UnicodeChar& b)
{
    return a < (int)b.get_code();
}
bool operator<(const UnicodeChar& a, const char& b)
{
    return (int)a.get_code() < b;
}
bool operator>(const char& a, const UnicodeChar& b)
{
    return a > (int)b.get_code();
}
bool operator>(const UnicodeChar& a, const char& b)
{
    return (int)a.get_code() > b;
}

bool operator<=(const char&a, const UnicodeChar& b)
{
    return a <= (int)b.get_code();
}
bool operator<=(const UnicodeChar& a, const char& b)
{
    return (int)a.get_code() <= b;
}
bool operator>=(const char& a, const UnicodeChar& b)
{
    return a >= (int)b.get_code();
}
bool operator>=(const UnicodeChar& a, const char& b)
{
    return (int)a.get_code() >= b;
}

bool operator<=(const int&a, const UnicodeChar& b)
{
    return a <= (int)b.get_code();
}
bool operator<=(const UnicodeChar& a, const int& b)
{
    return (int)a.get_code() <= b;
}
bool operator>=(const int& a, const UnicodeChar& b)
{
    return a >= (int)b.get_code();
}
bool operator>=(const UnicodeChar& a, const int& b)
{
    return (int)a.get_code() >= b;
}

bool operator==(const char& a, const UnicodeChar& b)
{
    return a == (int)b.get_code();
}
bool operator==(const UnicodeChar& a, const char& b)
{
    return (int)a.get_code() == b;
}
UnicodeChar operator+(const UnicodeChar& a, const int& b)
{
    return a.get_code() + b;
}
UnicodeChar operator+(const int& a, const UnicodeChar& b)
{
    return a + b.get_code();
}
UnicodeChar operator-(const UnicodeChar& a, const int& b)
{
    return a.get_code() - b;
}
UnicodeChar operator-(const int& a, const UnicodeChar& b)
{
    return a - b.get_code();
}
