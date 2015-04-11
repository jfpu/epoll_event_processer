#ifndef _JFPU_WORKER_H_
#define _JFPU_WORKER_H_

#include <string>

#include "msg.h"

using std::string;

namespace comm {

class Worker {
 public:
  Worker() {}
  virtual ~Worker() {}

  virtual bool DoMsg(Msg* msg) = 0;
  virtual void DropMsg(Msg* msg) = 0;

  virtual string Des() = 0;
};

class QueuedWorker : public Worker {
 public:
  virtual bool Enqueue(Msg* msg) = 0;
  virtual void DropAll() = 0;

  bool DoMsg(Msg* msg) {
    (void)msg;
    return true;
  }

  void DropMsg(Msg* msg) {
    if (NULL != msg) {
      delete msg;
    }
  }

  string Des() {
    return "QueueWorker";
  }
};

}

#endif
