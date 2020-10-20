#include "unicode_string.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

unsigned char g_first_byte_mark[] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
char g_utf8_bytes[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};
uint32_t g_utf8_offsets[] = {0x00000000UL, 0x00003080UL, 0x000E2080UL,
                             0x03C82080UL, 0xFA082080UL, 0x82082080UL
                            };

const char* g_space_token[] = {" ", "\t", "\n", "\r"};
const int g_space_token_size = 4;

UnicodeString::UnicodeString(const char* src) : _p_ref(NULL), _capacity(0),
    _byte_len(0), _char_len(0),
    _flag_char_len(0), _flag_is_null(1),
    _cursor_pos(-1), _cursor_point(NULL) {
    bzero(_spad, _SPAD_SIZE * sizeof(char));

    if (src == NULL) {
        return;
    }

    size_t src_len = strlen(src);

    if (src_len <= 0) { // 空字符
        return;
    }

    if (src_len < _SPAD_SIZE) {
        memcpy(_spad, src, src_len);
        _spad[src_len] = '\0';
    } else {
        if (re_allocate(src_len + 1)) {
            memcpy(_p_ref, src, src_len);
            _p_ref[src_len] = 0;
        } else {
            throw UnicodeStringReallocateError("UnicodeString reallocate Error");
        }
    }

    _flag_is_null = 0;
    _byte_len = src_len;
}

UnicodeString::UnicodeString(const UnicodeString& other) : _p_ref(NULL), _capacity(0),
    _byte_len(0), _char_len(0),
    _flag_char_len(0), _flag_is_null(1),
    _cursor_pos(-1), _cursor_point(NULL) {
    bzero(_spad, _SPAD_SIZE * sizeof(char));
    _byte_len = other._byte_len;
    _char_len = other._char_len;
    _flag_char_len = other._flag_char_len;
    _flag_is_null = other._flag_is_null;

    if (_flag_is_null) {
        return;
    }

    if (other._p_ref == NULL) {
        memcpy(_spad, other._spad, _SPAD_SIZE);
    } else {
        _capacity = other._capacity;

        if (_capacity <= 0) {
            _flag_is_null = 1;
            return;
        }

        _p_ref = new char[_capacity];
        memcpy(_p_ref, other._p_ref, _byte_len);
        _p_ref[_byte_len] = '\0';
    }
}

UnicodeString& UnicodeString::operator = (const UnicodeString& other) {
    _byte_len = other._byte_len;
    _char_len = other._char_len;
    _flag_char_len = other._flag_char_len;
    _flag_is_null = other._flag_is_null;
    _cursor_pos = other._cursor_pos;
    _cursor_point = other._cursor_point;

    if (_flag_is_null) {
        assign(NULL);
    } else if (other._p_ref == NULL) {
        assign(other._spad);
    } else {
        assign(other._p_ref);
    }

    return *this;
}

UnicodeString& UnicodeString::operator = (const char* src) {
    if (src == NULL) {  // 赋空指针，不做处理，维持现状
        return *this;
    }

    assign(src);
    return *this;
}

// 内部使用构造函数，不公开
UnicodeString::UnicodeString(const char* begin, const char* end) : _p_ref(NULL),
    _capacity(0),
    _byte_len(0), _char_len(0),
    _flag_char_len(0), _flag_is_null(1),
    _cursor_pos(-1), _cursor_point(NULL) {
    bzero(_spad, _SPAD_SIZE * sizeof(char));

    if (begin == NULL) {
        return;
    }

    size_t src_len = 0;

    if (end != NULL) {
        src_len = end - begin;
    } else {
        src_len = strlen(begin);
    }

    if (src_len <= 0) {
        return;
    }

    if (src_len < _SPAD_SIZE) {
        memcpy(_spad, begin, src_len);
        _spad[src_len] = 0;
    } else {
        if (re_allocate(src_len + 1)) {
            memcpy(_p_ref, begin, src_len);
            _p_ref[src_len] = 0;
        } else {
            throw UnicodeStringReallocateError("UnicodeString reallocate Error");
        }
    }

    _flag_is_null = 0;
    _byte_len = src_len;
}

void UnicodeString::clear() {
    bzero(_spad, _SPAD_SIZE * sizeof(char));

    if (_p_ref != NULL) {
        delete [] _p_ref;
        _p_ref = NULL;
        _capacity = 0;
    }

    _flag_is_null = 1;
    _byte_len = 0;
    _char_len = 0;
    _flag_char_len = 0;

    _cursor_pos = -1;
    _cursor_point = NULL;
}

void UnicodeString::assign(const char* src) {
    if (src == NULL) {
        bzero(_spad, _SPAD_SIZE * sizeof(char));

        if (_p_ref != NULL) {
            delete [] _p_ref;
            _p_ref = NULL;
            _capacity = 0;
        }

        _flag_is_null = 1;
        _byte_len = 0;
        _char_len = 0;
        _flag_char_len = 0;
    } else {
        size_t src_len = strlen(src);

        if (src_len <= 0) {
            bzero(_spad, _SPAD_SIZE * sizeof(char));

            if (_p_ref != NULL) {
                delete [] _p_ref;
                _p_ref = NULL;
                _capacity = 0;
            }

            _flag_is_null = 1;
            _byte_len = 0;
            _char_len = 0;
            _flag_char_len = 0;
        } else {
            if (NULL != _p_ref) {
                if (src_len < _SPAD_SIZE / 2) {
                    delete [] _p_ref;
                    _p_ref = NULL;
                    _capacity = 0;

                    snprintf(_spad, _SPAD_SIZE, "%s", src);
                } else {
                    if (re_allocate(src_len + 1)) {
                        memcpy(_p_ref, src, src_len);
                        _p_ref[src_len] = 0;
                    } else {
                        throw UnicodeStringReallocateError("UnicodeString reallocate Error");
                    }
                }
            } else {
                if (src_len < _SPAD_SIZE) {
                    memcpy(_spad, src, src_len);
                    _spad[src_len] = '\0';
                } else {
                    if (re_allocate(src_len + 1)) {
                        if (_p_ref != NULL) {
                            memcpy(_p_ref, src, src_len);
                            _p_ref[src_len] = 0;
                        }
                    } else {
                        throw UnicodeStringReallocateError("UnicodeString reallocate Error");
                    }
                }
            }

            _flag_is_null = 0;
            _byte_len = src_len;
            _flag_char_len = 0;
        }
    }

    _cursor_pos = -1;
    _cursor_point = NULL;
}

bool UnicodeString::re_allocate(unsigned int len) {
    if (_p_ref == NULL) {
        if (len < _SPAD_SIZE) {
            return false;
        } else {
            _capacity = _SPAD_SIZE * 2;

            while (_capacity <= len) {
                _capacity *= 2;
            }
        }

        _p_ref = new char[_capacity];
    } else {
        char* _t_p = NULL;

        if (len < _capacity / 4) {
            do {
                _capacity /= 2;
            } while (len < _capacity / 4);

            _t_p = new char[_capacity];
        } else if (len >= _capacity) {
            do {
                _capacity *= 2;
            } while (len >= _capacity);

            _t_p = new char[_capacity];
        }

        if (_t_p) {
            delete [] _p_ref;
            _p_ref = _t_p;
        }
    }

    return true;
}

UnicodeString::~UnicodeString() {
    if (_p_ref != NULL) {
        delete [] _p_ref;
        _p_ref = NULL;
    }

    _flag_is_null = 1;
}

bool UnicodeString::is_null() const {
    return _flag_is_null;
}

bool UnicodeString::empty() const {
    return _flag_is_null;
}

size_t UnicodeString::get_byte_len() const {
    if (_flag_is_null) {
        return 0;
    }

    return _byte_len;
}

size_t UnicodeString::get_len() const {
    if (_flag_is_null) {
        return 0;
    }

    if (!_flag_char_len) {
        _char_len = cal_len();
        _flag_char_len = 1;
    }

    return _char_len;
}

size_t UnicodeString::cal_len() const {
    size_t size = 0;
    char* begin = NULL;

    if (_p_ref != NULL) {
        begin = _p_ref;
    } else {
        begin = _spad;
    }

    uint8_t bytes = 0;

    while ((bytes = *begin)) {
        uint8_t c = g_utf8_bytes[bytes];

        switch (c) {
        case 5:          // illegal utf8
            return size;

        case 4:          // illegal utf8
            return size;

        case 3:
            size += 1;
            begin += 4;
            break;

        case 2:
            size += 1;
            begin += 3;
            break;

        case 1:
            size += 1;
            begin += 2;
            break;

        case 0:
            size += 1;
            begin += 1;
            break;
        }
    }

    return size;
}

const char* UnicodeString::c_str() const {
    if (_p_ref != NULL) {
        return _p_ref;
    } else {
        return _spad;
    }
}

void UnicodeString::strip(UnicodeString sp) {
    this->lstrip(sp);
    this->rstrip(sp);
}

void UnicodeString::rstrip(UnicodeString sp) {
    //if (sp.is_null()) {
    //    return;
    //}
    bool flag = false;

    if (sp.is_null()) {
        flag = true;
    }

    bool stop_flag = true;
    size_t len = this->get_len();
    size_t pos = len;
    size_t sp_len = sp.get_len();

    while (true) {
        if (flag) {
            stop_flag = true;

            for (int i = 0; i < g_space_token_size; i++) {
                UnicodeString cur_sep = g_space_token[i];
                size_t cur_sp_len = cur_sep.get_len();
                pos = pos - cur_sp_len;

                if (cur_sep == this->substr(pos, cur_sp_len)) {
                    // pos -= cur_sp_len;
                    stop_flag = false;
                    break;
                } else {
                    pos += cur_sp_len;
                }
            }

            if (stop_flag) {
                break;
            }
        } else {
            pos = pos - sp_len;

            if (sp == this->substr(pos, sp_len)) {
                // pos -= sp_len;
            } else {
                pos += sp_len;
                break;
            }
        }
    }
    (*this) = this->substr(0, pos);
}

void UnicodeString::lstrip(UnicodeString sp) {

    //if (sp.is_null()) {
    //    return;
    //}

    bool flag = false;

    if (sp.is_null()) {
        flag = true;
    }

    size_t pos = 0;

    bool stop_flag = true;

    while (true) {
        if (flag) {
            stop_flag = true;

            for (int i = 0; i < g_space_token_size; i++) {
                UnicodeString cur_sp = g_space_token[i];

                if (cur_sp == this->substr(pos, cur_sp.get_len())) {
                    pos += cur_sp.get_len();
                    stop_flag = false;
                    break;
                }
            }

            if (stop_flag) {
                break;
            }
        } else {
            if (sp == this->substr(pos, sp.get_len())) {
                pos += sp.get_len();
            } else {
                break;
            }
        }
    }

    (*this) = this->substr(pos);
}

std::vector<UnicodeString> UnicodeString::split(UnicodeString sep, int maxsplit) {
    std::vector<UnicodeString> to_ret;

    //if (sep.get_len() <= 0) {
    //    return to_ret;
    //}

    if (maxsplit == 0) {
        to_ret.push_back(*this);
        return to_ret;
    }

    size_t pos = 0;
    size_t n_pos = UnicodeString::npos;
    int count = 0;

    bool flag = false;
    UnicodeString cur_sep;

    if (sep.is_null()) {
        flag = true;
    } else {
        cur_sep = sep;
    }

    while (true) {
        if (flag) {
            size_t temp_pos = UnicodeString::npos;
            n_pos = UnicodeString::npos;

            for (int i = 0; i < g_space_token_size; i++) {
                temp_pos = find(g_space_token[i], pos);

                if (temp_pos != UnicodeString::npos) {
                    if (n_pos == UnicodeString::npos) {
                        n_pos = temp_pos;
                        cur_sep = g_space_token[i];
                    } else if (temp_pos < n_pos) {
                        n_pos = temp_pos;
                        cur_sep = g_space_token[i];
                    }
                }
            }
        } else {
            n_pos = find(cur_sep, pos);
        }

        if (n_pos ==
                pos) {      // 仅接着下个子串也是分割字符串。 在 flag为true时略过，否则，填入空字符串
            pos = pos + cur_sep.get_len();
            if (flag != true) {
                to_ret.push_back("");
                count++;
                if (maxsplit > 0 && count >= maxsplit) {
                    to_ret.push_back(substr(pos));
                    return to_ret;
                }
            }

        } else {
            if (n_pos == UnicodeString::npos) {
                if (pos == get_len() && flag != true) { // 当flag部位true是，需要考虑最后添加空字符
                    to_ret.push_back("");
                }
                if (pos < get_len()) {
                    to_ret.push_back(substr(pos));
                    return to_ret;
                } else {
                    return to_ret;
                }
            } else {
                if (flag) {    // flag 情况下，maxsplit判断放入到下次循环中，处理多个空格情况
                    if (maxsplit > 0 && count >= maxsplit) {
                        to_ret.push_back(substr(pos));
                        return to_ret;
                    }
                }
                to_ret.push_back(substr(pos, n_pos - pos));
                count++;
                pos = n_pos + cur_sep.get_len();

                if (!flag) { // 非 flag 情况，在此判断即可
                    if (maxsplit > 0 && count >= maxsplit) {
                        to_ret.push_back(substr(pos));
                        return to_ret;
                    }
                }
            }
        }
    }

    return to_ret;
}

std::vector<UnicodeChar> UnicodeString::split_char() const {
    std::vector<UnicodeChar> ret;
    int i = 0;

    while (true) {
        UnicodeChar cc = (*this) [i++];

        if (!cc.is_null()) {
            ret.push_back(cc);
        } else {
            break;
        }
    }

    return ret;
}

std::vector<UnicodeString> UnicodeString::split_char_s() {
    std::vector<UnicodeString> ret;
    int i = 0;

    while (true) {
        UnicodeChar cc = (*this)[i++];

        if (!cc.is_null()) {
            ret.push_back(cc.c_str());
        } else {
            break;
        }
    }

    return ret;
}

UnicodeString UnicodeString::join(std::vector<UnicodeString>& lists) {
    UnicodeString ret;
    int s_size = (int)lists.size() - 1;
    for (int i = 0; i < s_size; i++) {
        ret += lists[i];
        ret += *this;
    }
    if (lists.size()>0) {
        ret += lists[lists.size()-1];
    }

    return ret;
}

UnicodeString UnicodeString::join(std::vector<std::string>& lists) {
    UnicodeString ret;
    int s_size = (int)lists.size() - 1;
    for (int i = 0; i < s_size; i++) {
        ret += lists[i].c_str();
        ret += *this;
    }
    if (lists.size()>0) {
        ret += lists[lists.size()-1].c_str();
    }

    return ret;
}

UnicodeString UnicodeString::substr(size_t pos, size_t len) {
    if (pos >= get_len()) {
        return UnicodeString();
    }

    UnicodeChar temp_cc = (*this)[pos];
    char* begin = _cursor_point;
    char* end = NULL;

    if (len != UnicodeString::npos && pos + len < get_len()) {
        temp_cc = (*this)[pos + len];
        end = _cursor_point;
    }

    return UnicodeString(begin, end);
}

size_t UnicodeString::find(const UnicodeString& str, size_t pos) {
    UnicodeChar temp_cc;

    if (_flag_is_null) {
        return -1;
    }

    const char* s = str.c_str();
    size_t str_len = strlen(s);

    char* point = NULL;

    while (true) {
        temp_cc = (*this)[pos];

        if (temp_cc.is_null()) {
            return -1;
        }

        point = _cursor_point;

        if (0 == strncmp(s, point, str_len)) {
            return pos;
        }

        pos++;
    }

    return -1;
}

size_t UnicodeString::find(const char* s, size_t pos) {
    UnicodeChar temp_cc;

    if (_flag_is_null) {
        return -1;
    }

    size_t str_len = strlen(s);
    char* point = NULL;

    while (true) {
        temp_cc = (*this)[pos];

        if (temp_cc.is_null()) {
            return -1;
        }

        point = _cursor_point;

        if (0 == strncmp(s, point, str_len)) {
            return pos;
        }

        pos++;
    }

    return -1;
}

UnicodeString& UnicodeString::replace(size_t pos, size_t len, const UnicodeString& src) {
    if (pos >= get_len()) {
        return *this;
    }

    *this = this->substr(0, pos) + src + this->substr(pos + len);
    return *this;
}

UnicodeString& UnicodeString::replace(size_t pos, size_t len, const char* s) {
    if (pos >= get_len()) {
        return *this;
    }

    *this = this->substr(0, pos) + s + this->substr(pos + len);
    return *this;
}

UnicodeString& UnicodeString::replace(UnicodeString& src, UnicodeString& dest, int count) {
    int n = 0;
    size_t pos = 0;
    pos = find(src, pos);

    while (pos != UnicodeString::npos) {
        replace(pos, src.get_len(), dest);
        pos += dest.get_len();
        n++;

        if (count > 0 && n >= count) {
            break;
        }

        pos = find(src, pos);
    }

    return *this;
}

UnicodeString& UnicodeString::replace(const char* src, const char* dest, int count) {
    int n = 0;
    size_t pos = 0;
    size_t dest_len = strlen(dest);
    UnicodeString u_src = src;
    pos = find(src, pos);

    while ((int)pos != -1) {
        replace(pos, u_src.get_len(), dest);
        pos += dest_len;
        n++;

        if (count > 0 && n >= count) {
            break;
        }

        pos = find(src, pos);
    }

    return *this;
}

UnicodeString UnicodeString::lower() {
    UnicodeString ret;
    for (size_t i = 0; i < get_len(); i++) {
        UnicodeChar c = (*this)[i];
        if ('A' <= c && c <= 'Z') {
            c = c + 32;
        }
        ret = ret + c;
    }
    return ret;
}

UnicodeString UnicodeString::upper() {
    UnicodeString ret;
    for (size_t i = 0; i < get_len(); i++) {
        UnicodeChar c = (*this)[i];
        if ('a' <= c && c <= 'z') {
            c = c - 32;
        }
        ret = ret + c;
    }
    return ret;
}

bool UnicodeString::is_digit() {
    for (size_t i = 0; i < get_len(); i++) {
        UnicodeChar c = (*this)[i];
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

UnicodeChar UnicodeString::operator[](int pos) const {
    UnicodeChar cc;

    // support -1 -2 ... back index
    if (pos < 0) {
        pos = get_len() + pos;
    }

    if (pos < 0 || pos >= (int)get_len()) {
        return cc;
    }

    char* begin = NULL;

    int size = 0;

    if ((int)_cursor_pos != -1 && pos >= (int)_cursor_pos) {
        begin = _cursor_point;
        size = _cursor_pos;
    } else {
        if (_p_ref != NULL) {
            begin = _p_ref;
        } else {
            begin = _spad;
        }
    }

    uint8_t bytes = 0;
    char* bb = 0;
    char* ee = 0;

    while ((bytes = *begin)) {
        if (size == pos) {
            bb = begin;
        }

        uint8_t c = g_utf8_bytes[bytes];

        switch (c) {
        case 5:          // illegal utf8
            return cc;

        case 4:          // illegal utf8
            return cc;

        case 3:
            size += 1;
            begin += 4;
            break;

        case 2:
            size += 1;
            begin += 3;
            break;

        case 1:
            size += 1;
            begin += 2;
            break;

        case 0:
            size += 1;
            begin += 1;
            break;
        }

        if (size == pos + 1) {
            ee = begin;
            break;
        }
    }

    if (ee != NULL) {
        _cursor_pos = pos;
        _cursor_point = bb;
        cc.set_char(bb, ee);
    }

    return cc;
}

UnicodeString UnicodeString::operator+ (const UnicodeChar& other) const {
    if (this->is_null() && other.is_null()) {
        return UnicodeString();
    }

    size_t len = this->_byte_len + 1 + 5;
    char* temp = new char[len];

    snprintf(temp, len, "%s%s", this->c_str(), other.c_str());
    UnicodeString ret = UnicodeString(temp);
    delete [] temp;

    return ret;
}

UnicodeString UnicodeString::operator+ (const UnicodeString& other) const {
    if (this->is_null() && other.is_null()) {
        return UnicodeString();
    }

    size_t len = this->_byte_len + 1 + other._byte_len + 1;
    char* temp = new char[len];

    snprintf(temp, len, "%s%s", this->c_str(), other.c_str());
    UnicodeString ret = UnicodeString(temp);
    delete [] temp;

    return ret;
}

UnicodeString UnicodeString::operator+ (const char* src) const {
    size_t src_len = strlen(src);

    size_t len = this->_byte_len + 1 + src_len + 1;
    char* temp = new char[len];

    snprintf(temp, len, "%s%s", this->c_str(), src);
    UnicodeString ret = UnicodeString(temp);
    delete [] temp;

    return ret;
}

UnicodeString& UnicodeString::operator+= (const UnicodeChar& other) {
    UnicodeString ret = (*this) + other;
    (*this) = ret;
    return (*this);
}

UnicodeString& UnicodeString::operator+= (const UnicodeString& other) {
    UnicodeString ret = (*this) + other;
    (*this) = ret;
    return (*this);
}

bool UnicodeString::operator== (const UnicodeString& other) const {
    const char* p1 = this->c_str();
    const char* p2 = other.c_str();
    return !strcmp(p1, p2);
}

bool UnicodeString::operator!= (const UnicodeString& other) const {
    const char* p1 = this->c_str();
    const char* p2 = other.c_str();
    return strcmp(p1, p2);
}

bool UnicodeString::operator< (const UnicodeString& other) const {
    const char* p1 = this->c_str();
    const char* p2 = other.c_str();
    return strcmp(p1, p2) < 0 ? true : false;
}

UnicodeString& UnicodeString::operator+= (const char* src) {
    UnicodeString ret = (*this) + src;
    (*this) = ret;
    return (*this);
}

UnicodeString operator+ (const UnicodeChar& a, const UnicodeChar& b) {
    char temp[10];

    if (a.is_null() && b.is_null()) {
        return UnicodeString();
    }

    snprintf(temp, 10, "%s%s", a.c_str(), b.c_str());
    return UnicodeString(temp);
}

UnicodeString operator+ (const char* a, const UnicodeString& b) {
    if (a == NULL) {
        return b;
    }

    size_t src_len = strlen(a);

    size_t len = b.get_byte_len() + 1 + src_len + 1;
    char* temp = new char[len];

    snprintf(temp, len, "%s%s", a, b.c_str());
    UnicodeString ret = UnicodeString(temp);
    delete [] temp;

    return ret;
}

bool operator== (const char* a, const UnicodeString& b) {
    const char* p1 = a;
    const char* p2 = b.c_str();
    return !strcmp(p1, p2);
}

bool operator!= (const char* a, const UnicodeString& b) {
    const char* p1 = a;
    const char* p2 = b.c_str();
    return strcmp(p1, p2);
}
