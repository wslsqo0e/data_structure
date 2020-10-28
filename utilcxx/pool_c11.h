/*************************************************************************
> File Name:  pool_c11.h
> Author: shenming
> Created Time: Fri Sep 25 12:25:13 2020
> 兼容C++11标准的 thread pool
************************************************************************/

#ifndef __UTILCXX_POOL_C11_H__
#define __UTILCXX_POOL_C11_H__

#include <future>
#include "base_utils.h"
#include "queue_c11.h"

namespace utilcxx {

class ThreadTask {
public:
  virtual void handle() {};
  virtual ~ThreadTask() {};
private:
};

class SimpleThreadPool
{
public:
  explicit SimpleThreadPool(unsigned int threads = std::thread::hardware_concurrency())
  {
    if (!threads) {
      throw std::invalid_argument("Invalid thread count!");
    }

    auto worker = [this]()
                  {
                    while (true) {
                      ThreadTask *task = NULL;
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

  ~SimpleThreadPool()
  {
    m_queue.done();
    // 会确保m_queue为空的前提下才会退出所有线程
    for(auto& thread : m_threads)
      thread.join();
  }

  void enqueue_work(ThreadTask *task) {
    m_queue.push(task);
  }

private:
  using Proc = std::function<void(void)>;
  // using Queue = blocking_queue<Proc>;
  using Queue = BlockingQueue<ThreadTask*>;
  Queue m_queue;

  using Threads = std::vector<std::thread>;
  Threads m_threads;
};

class ThreadPool
{
public:
  explicit ThreadPool(unsigned int threads = std::thread::hardware_concurrency())
    : m_queues(threads), m_count(threads), m_index(0)
  {
    if(!threads)
      throw std::invalid_argument("Invalid thread count!");

    auto worker = [this](size_t i)
                  {
                    while(true) {
                      ThreadTask *task = NULL;
                      for(auto n = 0; n < m_count * K; ++n)
                        if(m_queues[(i + n) % m_count].try_pop(task))  // 从多个block queue 中获取
                          break;
                      if(!task && !m_queues[i].pop(task))
                        break;
                      task->handle();
                      if (task) {
                        delete task;  // 析构在此处
                      }
                    }
                  };
    for (auto i = 0; i < threads; ++i) {
      m_threads.emplace_back(worker, i);
    }
  }

  ~ThreadPool() {
    for(auto& queue : m_queues)
      queue.done();
    for(auto& thread : m_threads)
      thread.join();
  }

  void enqueue_work(ThreadTask *task) {
    auto i = m_index++;
    for (auto n = 0; n < m_count * K; ++n) {
      if(m_queues[(i + n) % m_count].try_push(task))
        return;
    }
    m_queues[i % m_count].push(task);
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
  using Proc = std::function<void(void)>;
  using Queue = BlockingQueue<ThreadTask*>;
  using Queues = std::vector<Queue>;
  Queues m_queues;   // 多个 block queues 的队列

  using Threads = std::vector<std::thread>;
  Threads m_threads;

  const unsigned int m_count;
  std::atomic_uint m_index;

  // inline static const unsigned int K = 2;
  static const unsigned int K = 2;
};

}


#endif
