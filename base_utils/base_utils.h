/*************************************************************************
> File Name:  base_utils.h
> Author: shenming
> Created Time: Tue Oct 20 18:46:13 2020
************************************************************************/

#ifndef __BASE_UTILS_H__
#define __BASE_UTILS_H__

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <algorithm>
#include "unicode_string.h"

template<typename T>
inline void print_vec(std::vector<T> vv) {
  for (auto &v:vv) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}

template<typename T>
void delete_ptr_vec(std::vector<T> vec)
{
  for (typename std::vector<T>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
    delete *iter;
  }

  std::vector<T> tmp;
  tmp.swap(vec);
};

inline std::string get_cur_thread_id() {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  std::string stid = oss.str();
  // uint64_t tid = std::stoull(stid);
  // return tid;
  return stid;
}

inline std::string path_join(const std::string &data_path, const std::string &file_name) {
  return data_path + "/" + file_name;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                                                        return !std::isspace(ch);
                                                      }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                                               return !std::isspace(ch);
                                             }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

// 半角转全角
std::string Q2B(const std::string &input);
// 全角转半角
std::string B2Q(const std::string &input);


std::string remove_comment(const std::string &input_str);

std::vector<std::string> string_split(std::string str, std::string delimiter);

// Makes copy constructor and operator= private.
#define DISALLOW_COPY_AND_ASSIGN(type)          \
  type(const type&);                            \
  void operator = (const type&)

#endif
