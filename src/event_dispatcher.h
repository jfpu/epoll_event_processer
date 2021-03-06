#ifndef _JFPU_EVENT_DISPATCHER_H
#define _JFPU_EVENT_DISPATCHER_H

#include <cstddef>
#include <unistd.h>

#include "log.h"
#include "atomic.h"
#include "msg.h"
#include "event_worker.h"

namespace comm {

class EventDispatcher {
 public:
  EventDispatcher() : num_(0),  ew_(NULL) {
    loc_ = ATOMIC_INIT(0);
  }

  bool Init(uint32_t num, EventWorker* ew) {
    #if 0
    ew_ = new EventWorker[num_];
    if (NULL == ew_) {
      LogError("new %d EventWorker failed!", num_);
      return false;
    }
    #endif

    num_ = num;
    ew_ = ew;
    for (uint32_t i = 0; i < num_; ++i) {
      EventWorker* cew = ew_ + i;
      if (!(cew->Init())) {
        LogError("the %d EventWorker Init failed!", i);
        return false;
      }
    }

    return true;
  }

  void UnInit() {
    for (uint32_t i = 0; i < num_; ++i) {
      EventWorker* cew = ew_ + i;
      cew->UnInit();
    }

    #if 0
    delete [] ew_;
    #endif

    ew_ = NULL;
  }

  bool Enqueue(Msg* msg) {
    uint32_t cl = atomic_read(&loc_);
    atomic_inc(&loc_);
    EventWorker* cew = ew_ + (cl % num_);
    cew->Enqueue(msg);
    return true;
  }

 private:
  uint32_t num_;
  atomic_t loc_;
  EventWorker* ew_;
};

}

#endif
