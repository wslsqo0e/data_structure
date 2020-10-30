/*************************************************************************
> File Name:  time_cost.h
> Author: shenming
> Created Time: Fri Sep 25 12:32:52 2020
************************************************************************/

#ifndef __UTILCXX_TIME_COST_H__
#define __UTILCXX_TIME_COST_H__
#include <string>
#include <iostream>
#include <chrono>

namespace utilcxx {

class TimeCost {
public:
  TimeCost(std::string message = "", bool is_auto = false) : message_(message), is_auto_(is_auto) {
    start_time = std::chrono::high_resolution_clock::now();
  }

  ~TimeCost() {
    if (is_auto_) {
      duration();
      }
  }

  void reset() {
    start_time = std::chrono::high_resolution_clock::now();
  }

  // 返回毫秒值
  unsigned int get_reset() {
    auto end_time = std::chrono::high_resolution_clock::now();
    start_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  }

  void duration() {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << message_ << "\ttime cost: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.f << std::endl;
  }

  void duration(std::string message) {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << message << "\ttime cost: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.f << std::endl;
  }

private:
  bool is_auto_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::string message_;
};

}

#endif
