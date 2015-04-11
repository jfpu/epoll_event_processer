#ifndef _JFPU_MUTEX_H_
#define _JFPU_MUTEX_H_

#include <pthread.h>

namespace comm {

class Mutex {
 public:
  Mutex() {
    pthread_mutex_init(&mutex_, NULL);
  }

  ~Mutex() {
    pthread_mutex_destroy(&mutex_);
  }

  void Lock() {
    pthread_mutex_lock(&mutex_);
  }

  void UnLock() {
    pthread_mutex_unlock(&mutex_);
  }

  pthread_mutex_t* GetMutex() {
    return &mutex_;
  }

 private:
  Mutex(const Mutex&);
  const Mutex& operator=(const Mutex&);

 private:
  pthread_mutex_t mutex_;
};

class Guard {
 public:
  explicit Guard(Mutex* lock) : pointer_(lock) {
    pointer_->Lock();
  }

  ~Guard() {
    pointer_->UnLock();
  }

 private:
  Guard();
  const Guard& operator=(const Guard&);

 private:
  Mutex* pointer_;
};

}

#endif
