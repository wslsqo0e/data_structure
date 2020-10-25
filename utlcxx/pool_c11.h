/*************************************************************************
> File Name:  pool_c11.h
> Author: shenming
> Created Time: Fri Sep 25 12:25:13 2020
> 兼容C++11标准的 thread pool
************************************************************************/

#ifndef __UTILCXX_POOL_C11_H__
#define __UTILCXX_POOL_C11_H__

#include <future>
#include "queue_c11.h"

namespace utilcxx {

class thread_task {
public:
  virtual void handle() {};
  virtual ~thread_task() {};
private:
};

class simple_thread_pool
{
public:
  explicit simple_thread_pool(unsigned int threads = std::thread::hardware_concurrency())
  {
    if (!threads) {
      throw std::invalid_argument("Invalid thread count!");
    }

    auto worker = [this]()
                  {
                    while (true) {
                      thread_task *task = NULL;
                      if (!m_queue.pop(task))   // 可能阻塞，知道 blocking_queue:m_done 为True时，线程才有可能退出
                        break;
                      task->handle();
                      if (task) {
                        delete task; // 析构在此处
                      }
                    }
                  };

    for (auto i = 0; i < threads; ++i) {
      m_threads.emplace_back(worker);
    }
  }

  ~simple_thread_pool()
  {
    m_queue.done();
    // 会确保m_queue为空的前提下才会退出所有线程
    for(auto& thread : m_threads)
      thread.join();
  }

  void enqueue_work(thread_task *task) {
    m_queue.push(task);
  }

private:
  using Proc = std::function<void(void)>;
  // using Queue = blocking_queue<Proc>;
  using Queue = blocking_queue<thread_task*>;
  Queue m_queue;

  using Threads = std::vector<std::thread>;
  Threads m_threads;
};

}


#endif
