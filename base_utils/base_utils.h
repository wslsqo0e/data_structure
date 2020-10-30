/*************************************************************************
> File Name:  base_utils.h
> Author: shenming
> Created Time: Tue Oct 20 18:46:13 2020

  定义一些常用的函数，暂时放置在Logger模块中
************************************************************************/

#ifndef __BASE_UTILS_H__
#define __BASE_UTILS_H__

#include <vector>
#include <iostream>

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

// Makes copy constructor and operator= private.
#define DISALLOW_COPY_AND_ASSIGN(type)          \
  type(const type&);                            \
  void operator = (const type&)

#endif