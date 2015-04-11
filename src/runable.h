#ifndef _JFPU_RUNABLE_H_
#define _JFPU_RUNABLE_H_

#include <unistd.h>
#include <pthread.h>

#include "atomic.h"

namespace comm {

class NoCopy {
  NoCopy(const NoCopy&);
  NoCopy& operator=(const NoCopy&);
 public:
  NoCopy() {}
  virtual ~NoCopy() {}
#if 0
 public:
  NoCopy() = default;
  ~NoCopy() = default;
  NoCopy(const NoCopy&) = delete;
  NoCopy& operator=(const NoCopy&) = delete;
#endif
};

class Runable : public NoCopy {
  atomic_t state_;    // Executive runner state

  enum RunState {
    RUNNING = 0xDCBA,
    STOPPING,
    STOPPED,
  };

  int __GetThreadState() const {
    return atomic_read(&state_);
  }

  void __SetThreadState(RunState st) {
    atomic_set(&state_, (int)st);
  }

  static void __Cleanup(Runable* runner) {
    runner->__SetThreadState(STOPPED);
  }

 public:
  Runable() {
    __SetThreadState(RUNNING);
  }

  virtual ~Runable() {}

  void SetStopping() {
    __SetThreadState(STOPPING);
  }

  bool Running() const {
    return ((RUNNING == __GetThreadState()) ? true : false);
  }

  bool Stopped() const {
    return ((STOPPED == __GetThreadState()) ? true : false);
  }

  // Calling Prerequisite: Runner should running first
  void WaitStop() {
    // Avoid set STOPPING before the thead Running
    while (Running()) {
      SetStopping();
      usleep(500);
    }

    while (!Stopped()) {
      usleep(500);
    }
  }

  // Better to judge a cancel point: pthread_testcancel();
  virtual void RealExecute() = 0;

  void Execute() {
    // Set Thread Cancel State and Type
    int oldstate = 0;
    int oldtype = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);

    pthread_cleanup_push((void (*)(void*)) __Cleanup, (void*) this);
    {
      __SetThreadState(RUNNING);
      RealExecute();
      __SetThreadState(STOPPED);
    } pthread_cleanup_pop(0);
  }
};

}

#endif
