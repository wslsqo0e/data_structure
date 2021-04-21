/*************************************************************************
> File Name:  time_cost-test.cc
> Author: shenming
> Created Time: Wed Apr  7 22:12:57 2021
************************************************************************/

#include "time_cost.h"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
  utilcxx::TimeCost tc;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  tc.duration("hello world");
  return 0;
}
