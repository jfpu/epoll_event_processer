#ifndef _JFPU_THREAD_POOL_H_
#define _JFPU_THREAD_POOL_H_

#include <iostream>
#include <vector>
#include <cstddef>
#include <stdint.h>
#include <unistd.h>

#include "likely.h"
#include "thread.h"
// #include "def.h"

namespace comm {

class ThreadPool : public NoCopy {
 protected:
  std::vector<Thread*> tvec_;

 public:
  virtual ~ThreadPool() {}

  // Request: (NULL != runner && 0 < num)
  virtual bool Init(uint32_t num) = 0;

  void Run() {
    for (size_t i = 0; i < tvec_.size(); ++i) {
      if (unlikely(NULL == tvec_[i]))
        continue;

      tvec_[i]->Run();
    }
  }

  void WaitStop() {
    for (size_t i = 0; i < tvec_.size(); ++i) {
      if (unlikely(NULL == tvec_[i]))
        continue;

      tvec_[i]->WaitStop();
    }
  }

  void Stop() {
    for (size_t i = 0; i < tvec_.size(); ++i) {
      if (unlikely(NULL == tvec_[i]))
        continue;

      tvec_[i]->Stop();
    }
  }

  void SecureStop() {
    Stop();
    WaitStop();
  }

  // Delete each thread after its secure exit
  void UnInit() {
    for (size_t i = 0; i < tvec_.size(); ++i) {
      if (unlikely(NULL == tvec_[i]))
        continue;

      delete tvec_[i];
      tvec_[i] = NULL;
    }
    tvec_.clear();
  }
};

}

#endif
