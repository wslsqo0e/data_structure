/*
  DartsImpl的封装，方便使用
*/
#pragma once
#include "darts.h"
#include "Logger/log_spd.h"

namespace DARTS {

// DoubleArrayImpl的封装
// 内部留有自己的一份数据copy _pair_members
// 并且进行排序
class TrieDarts {
public:
  TrieDarts() : _is_build(false) {}
  TrieDarts(size_t key_size, char** key):_is_build(false) {
    for (int i = 0; i < key_size; i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(key[i], i));
    }
  }

  TrieDarts(std::vector<const char*>& members):_is_build(false) {
    size_t count = 0;
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(*i, count));
      count += 1;
    }
  }
  TrieDarts(std::vector<std::string>& members):_is_build(false) {
    size_t count = 0;
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(*i, count));
      count += 1;
    }
  }

  TrieDarts(std::vector<std::pair<const char*, unsigned int>>& members):_is_build(false) {
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(i->first, i->second));
    }
  }

  TrieDarts(std::vector<std::pair<std::string, unsigned int>>& members):_is_build(false) {
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(i->first, i->second));
    }
  }

  void insert(std::vector<std::string>& members) {
    size_t count = 0;
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(*i, count));
      count += 1;
    }
  }

  void insert(const char* member) {
    if (_pair_members.empty()) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(member, 0));
      return;
    }
    size_t value = _pair_members[_pair_members.size() - 1].second + 1;
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, value));

  }
  void insert(const char* member, size_t value) {
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, value));
  }

  void insert(const std::string& member) {
    if (_pair_members.empty()) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(member, 0));
      return;
    }
    size_t value = _pair_members[_pair_members.size() - 1].second + 1;
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, value));
  }

  void insert(const std::string& member, size_t value) {
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, value));
  }

  int build() {
    if (_is_build) {
      spdlog::error("TrieDarts already build");
      return -1;
    }
    if (_pair_members.size() == 0) {
      spdlog::error("you are trying to build a empty trie");
      return -1;
    }
    // sort first
    std::sort(_pair_members.begin(), _pair_members.end());
    char **key = new char* [_pair_members.size()];
    int * value = new int [_pair_members.size()];

    size_t count = 0;
    for (auto i = _pair_members.begin(); i != _pair_members.end(); i++) {
      key[count] = (char*)(i->first.c_str());
      value[count] = i->second;
      count += 1;
    }

    int error = _array.build(_pair_members.size(), key, value);
    delete [] key;
    delete [] value;

    if (error != 0) {
      spdlog::error("build TrieDarts faled, you may have duplicates keys in data");
      return error;
    }

    _is_build = true;

    return 0;
  }

  ~TrieDarts() {
    clear();
  };

  // 搜索key，返回对应value
  int exactMatchSearch(const char* key) {
    if (!_is_build) return -1;
    return _array.exactMatchSearch<DARTS::DoubleArray::result_type>(key);
  }
  int exactMatchSearch(const std::string& key) {
    if (!_is_build) return -1;
    return _array.exactMatchSearch<DARTS::DoubleArray::result_type>(key.c_str());
  }

  // 搜索最长前缀，返回对应value
  int prefixSearch(const char* key) {
    if (!_is_build) return -1;
    return _array.prefixSearch<DARTS::DoubleArray::result_type>(key);
  }
  int prefixSearch(const std::string &key) {
    if (!_is_build) return -1;
    return _array.prefixSearch<DARTS::DoubleArray::result_type>(key.c_str());
  }

  // 搜索Contain的最长长度，返回长度
  size_t containLen(const char* key, bool& is_word) {
    if (!_is_build) return -1;
    return _array.containLen<DARTS::DoubleArray::result_type>(key, is_word);
  }
  size_t containLen(const std::string &key, bool& is_word) {
    if (!_is_build) return -1;
    return _array.containLen<DARTS::DoubleArray::result_type>(key.c_str(), is_word);
  }

  // 搜索darts中key所有的前缀
  // return 前缀个数
  // result 数组存储对应的value值。
  size_t commonPrefixSearch(const char* key, int *result, size_t result_len) {
    if (!_is_build) return -1;
    return _array.commonPrefixSearch<DARTS::DoubleArray::result_type>(key, result, result_len);
  }
  size_t commonPrefixSearch(const std::string &key, int *result, size_t result_len) {
    if (!_is_build) return -1;
    return _array.commonPrefixSearch<DARTS::DoubleArray::result_type>(key.c_str(), result, result_len);
  }

  void clear() {
    std::vector<std::pair<std::string, unsigned int>> tmp;
    tmp.swap(_pair_members);
    _array.clear();
    _is_build = false;
  };

  int save_dart(std::ofstream& ofs) {
    if (!_is_build) {
      return -1;
    }

    if (! ofs.is_open()) {
      return 1;
    }

    char sep = '\0';

    size_t size = _pair_members.size();
    ofs.write((char*)&size, sizeof(size_t));

    for (size_t i = 0; i < size; i++) {
      const char* key = _pair_members[i].first.c_str();
      unsigned int wid = _pair_members[i].second;
      ofs.write(key, strlen(key));
      ofs.write(&sep, sizeof(sep));
      ofs.write((char*)&wid, sizeof(unsigned int));
    }

    return _array.save_dart(ofs);
  }

  int load_dart(std::ifstream& ifs) {
    if (! ifs.is_open()) {
      return 1;
    }

    clear();

    size_t size;
    ifs.read((char*)&size, sizeof(size_t));
    for (size_t i = 0; i < size; i++) {
      unsigned int wid = 0;
      char ch = 0;
      std::string key = "";
      while (true) {
        ifs.read(&ch, sizeof(ch));
        if (ch == '\0') {
          break;
        }
        key.push_back(ch);
      }
      ifs.read((char*)&wid, sizeof(unsigned int));
      _pair_members.push_back(std::pair<std::string, unsigned int>(key, wid));
    }

    int ret = _array.load_dart(ifs);
    if (ret == 0) {
      _is_build = true;
    }
    return ret;
  }

protected:
  bool _is_build;
  // bool _is_first;

  DoubleArray _array;

  std::vector<std::pair<std::string, unsigned int>> _pair_members;
};

}
