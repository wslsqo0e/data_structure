#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include<assert.h>
#include<pthread.h>

template<typename T>
class Singleton {
public:
  static  T * instance() {
    if(obj_instance == NULL) {
      pthread_mutex_lock(&mutex_);
      if(obj_instance == NULL) {
        obj_instance = new T;
      }
      pthread_mutex_unlock(&mutex_);
    }

    assert(obj_instance != NULL);
    return obj_instance;
  }

protected:
  Singleton () {}
  virtual ~Singleton() {}

private:
  Singleton (const Singleton &) ;

  Singleton & operator = (const Singleton &);

  static pthread_mutex_t mutex_;
  static T * obj_instance;
};

template<typename T> pthread_mutex_t Singleton<T>::mutex_ = PTHREAD_MUTEX_INITIALIZER;
template<typename T> T* Singleton<T>::obj_instance = NULL;

#endif
