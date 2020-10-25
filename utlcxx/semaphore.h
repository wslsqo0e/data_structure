/*************************************************************************
> File Name:  semaphore.h
> Author: shenming
> Created Time: Thu Sep 17 12:20:55 2020
************************************************************************/

#ifndef __UTILCXX_SEMAPHORE_H__
#define __UTILCXX_SEMAPHORE_H__

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

namespace utilcxx {

class semaphore
{
public:
  explicit semaphore(unsigned int count = 0)
    : m_count(count)
  {}

  void post()
  {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      ++m_count;
    }
    m_cv.notify_one();
  }

  void post(unsigned int count)
  {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_count += count;
    }
    m_cv.notify_all();
  }

  void wait()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this]() { return m_count != 0; });
    --m_count;
  }

  template<typename Rep, typename Period>
  bool wait_for(const std::chrono::duration<Rep, Period>& t)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(!m_cv.wait_for(lock, t, [this]() { return m_count != 0; }))
      return false;
    --m_count;
    return true;
  }

  template<typename Clock, typename Duration>
  bool wait_until(const std::chrono::time_point<Clock, Duration>& t)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(!m_cv.wait_until(lock, t, [this]() { return m_count != 0; }))
      return false;
    --m_count;
    return true;
  }

private:
  std::atomic_int m_count;
  std::mutex m_mutex;
  std::condition_variable m_cv;
};

class fast_semaphore
{
public:
  explicit fast_semaphore(unsigned int count = 0)
    : m_count(count), m_semaphore(0) {}

  void post()
  {
    int count = m_count.fetch_add(1, std::memory_order_release); // 返回 m_count 原来的值
    if (count < 0)
      m_semaphore.post();
  }

  void wait()
  {
    int count = m_count.fetch_sub(1, std::memory_order_acquire);
    if (count < 1)
      m_semaphore.wait();
  }

private:
  std::atomic_int m_count;
  semaphore m_semaphore;
};

}

#endif
