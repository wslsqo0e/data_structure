#include <stdio.h>
#include <iostream>
#include <sstream>
#include "pool.h"
#include "ascii_escape_code.h"

using namespace utilcxx;
using namespace std;
using namespace std::chrono;
using namespace ascii_escape_code;

// 为数字添加逗号  100000 -> 100,000
std::string with_commas(uint64_t value)
{
  std::stringstream ss;
  ss.imbue(std::locale(""));
  ss << std::fixed << value;
  return ss.str();
}

template<typename PT>
void benchmark(const char *pool_name, uint64_t tasks, uint64_t reps)
{
  cout << black << bold << pool_name << normal << " (" << red <<
  with_commas(tasks) << black << " tasks, " << red <<
  with_commas(reps) << black << " reps)" << flush;

  atomic_uint64_t result = 0;
  auto work = [&result](uint64_t r)
              {
                uint64_t sum = 0;
                for (auto i = 1; i <= r; ++i) {
                  auto t = rand();
                  sum += t + 1;
                  sum -= t;
                }
                result += sum;     // 操作外部的唯一变量
              };

  auto start_time = high_resolution_clock::now();
  {
    PT pool;
    for (uint64_t i = 1; i <= tasks / 2; ++i)
      {
        pool.enqueue_work(work, reps);
        [[maybe_unused]] auto p = pool.enqueue_task(work, reps);
      }
  }
  auto end_time = high_resolution_clock::now();

  cout << "\t" << red << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << black;
  cout << "\t(", (result == tasks * reps ? cout << green : cout << red), cout << with_commas(result) << reset << ")" << endl;
}

void pool_test()
{
  uint64_t TASK_START = 100'000;   // 第一次知道c++支持这种写法啊
  uint64_t TASK_STEP  = 100'000;
  uint64_t TASK_STOP  = 1'000'000;

  uint64_t REPS_START = 100;
  uint64_t REPS_STEP  = 100;
  uint64_t REPS_STOP  = 1'000;

  for (auto t = TASK_START; t <= TASK_STOP; t += TASK_STEP) {
    if(TASK_START < TASK_STOP)
      std::cout << "********************************************************************************" << std::endl;
    for (auto r = REPS_START; r <= REPS_STOP; r += REPS_STEP) {
      benchmark<simple_thread_pool> ("simple  ", t, r);
      benchmark<thread_pool>        ("advanced", t, r);
      if(REPS_START < REPS_STOP) cout << endl;
    }
  }

  return;
}

int main(int argc, char *argv[])
{
  pool_test();
  printf("Yes\n");
}
