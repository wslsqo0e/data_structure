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

#endif
