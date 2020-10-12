/*
  TrieDarts的封装，用于字符串数组的搜索和匹配
  字典树每个节点存储的都是char，不然搜索太宽泛，生成darts也不高效
*/
#pragma once
#include "trie_darts.h"
#include <unordered_map>

namespace DARTS {
static const std::string DARTS_MAGIC = "DARTS";
class StrDarts : public TrieDarts {
public:
  StrDarts() : _word_count(0) {};
  ~StrDarts() {
    _word_map.clear();
  }
  int init(std::vector<std::string>& members) {
    if (_is_build) {
      spdlog::error("already build");
      return -1;
    }
    _word_count = 0;
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(*i, _word_count));
      _word_map[_word_count++] = *i;
    }
    return 0;
  }
  int init(std::vector<const char*>& members) {
    if (_is_build) {
      spdlog::error("already build");
      return -1;
    }
    _word_count = 0;
    for (auto i = members.begin(); i != members.end(); i++) {
      _pair_members.push_back(std::pair<std::string, unsigned int>(*i, _word_count));
      _word_map[_word_count++] = *i;
    }
    return 0;
  }
  int insert(std::string& member) {
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, _word_count));
    _word_map[_word_count++] = member;
    return 0;
  }
  int insert(const char* member) {
    _pair_members.push_back(std::pair<std::string, unsigned int>(member, _word_count));
    _word_map[_word_count++] = member;
    return 0;
  }
  // build 继承父类方法
  // int build();

  std::string getLongestPrefix(const std::string &key) {
    if (!_is_build) return "";
    int ret = prefixSearch(key);
    if (ret == -1) {
      return "";
    } else {
      return _word_map[ret];
    }
  }

  std::string getLongestPrefix(const char* key) {
    if (!_is_build) return "";
    int ret = prefixSearch(key);
    if (ret == -1) {
      return "";
    } else {
      return _word_map[ret];
    }
  }

  bool isMatch(const std::string &key) {
    if (_is_build) return false;
    int ret = exactMatchSearch(key);
    if (ret == -1) {
      return false;
    } else {
      return true;
    }
  }

  bool isMatch(const char *key) {
    if (_is_build) return false;
    int ret = exactMatchSearch(key);
    if (ret == -1) {
      return false;
    } else {
      return true;
    }
  }

  // token 匹配，返回索引，指代token[start:end]在字典树中存在并成词， 不包括返回的end
  size_t match_token(std::vector<std::string>& tokens, size_t start = 0) {
    if (start >= tokens.size()) {
      return tokens.size();
    }
    std::string whole_tokens;
    for (size_t i = start; i < tokens.size(); i++) {
      whole_tokens += tokens[i];
    }
    std::string word_match = getLongestPrefix(whole_tokens);
    size_t len = word_match.size();
    if (len == 0) {
      return start;
    }
    for (size_t i = start; i < tokens.size(); i++) {
      if (tokens[i].size() <= len) {
        len -= tokens[i].size();
        continue;
      } else {
        return i;
      }
    }
    return tokens.size();
  }

  size_t contain_tokens(std::vector<std::string>& tokens, bool& is_word, size_t start = 0) {
    is_word = false;
    if (start >= tokens.size()) {
      return tokens.size();
    }
    std::string whole_tokens;
    for (size_t i = start; i < tokens.size(); i++) {
      whole_tokens += tokens[i];
    }
    size_t contain_len = containLen(whole_tokens, is_word);
    for (size_t i = start; i < tokens.size(); i++) {
      if (tokens[i].size() <= contain_len) {
        contain_len -= tokens[i].size();
        continue;
      } else {
        return i;
      }
    }
    return tokens.size();
  }

  int save_dart(std::ofstream& ofs) {
    if (!_is_build) {
      return -1;
    }

    if (! ofs.is_open()) {
      return 1;
    }

    int skip = 0;
    ofs.write(DARTS_MAGIC.c_str(), 4);
    ofs.write((char*)&skip, sizeof(int));
    ofs.write((char*)&skip, sizeof(int));
    ofs.write((char*)&skip, sizeof(int));

    ofs.write((char*)&_word_count, sizeof(size_t));

    char sep = '\0';
    for (auto i = _word_map.begin(); i != _word_map.end(); i++) {
      const char* key = i->second.c_str();
      int wid = i->first;
      ofs.write(key, strlen(key));
      ofs.write(&sep, sizeof(sep));
      ofs.write((char*)&wid, sizeof(int));
    }

    return TrieDarts::save_dart(ofs);
  }

  int load_dart(std::ifstream& ifs) {
    if (! ifs.is_open()) {
      return 1;
    }

    int skip = 0;
    char magic[4 + 1] = {0};
    ifs.read(magic, 4);
    magic[4] = '\0';
    if (strncmp(magic, DARTS_MAGIC.c_str(), 4) != 0) {
      spdlog::warn("invalid dart file format");
      return -1;
    }

    ifs.read((char*)&skip, sizeof(int));
    ifs.read((char*)&skip, sizeof(int));
    ifs.read((char*)&skip, sizeof(int));

    ifs.read((char*)&_word_count, sizeof(size_t));
    _word_map.clear();
    for (int i = 0; i < _word_count; i++) {
      int wid = 0;
      char ch = 0;
      std::string key = "";

      while (true) {
        ifs.read(&ch, sizeof(ch));

        if (ch == '\0') {
          break;
        }
        key.push_back(ch);
      }
      ifs.read((char*)&wid, sizeof(int));
      _word_map[wid] = key;
    }
    return TrieDarts::load_dart(ifs);
  }

  int save(const char* file_name) {
    std::ofstream ofs(file_name, std::ios::binary);
    if (! ofs.is_open()) {
      spdlog::warn("invalid filename {}", file_name);
      return 1;
    }

    return save_dart(ofs);
  }

  int load(const char* file_name) {
    std::ifstream ifs(file_name, std::ios::binary);
    if (! ifs.is_open()) {
      spdlog::warn("invalid filename {}", file_name);
      return 1;
    }

    return load_dart(ifs);
  }

private:
  std::unordered_map<int, std::string> _word_map;
  size_t _word_count;
};

}
