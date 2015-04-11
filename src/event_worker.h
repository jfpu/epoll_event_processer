#ifndef _JFPU_EVENT_WORKER_H_
#define _JFPU_EVENT_WORKER_H_

#include <vector>

#include "msg.h"
#include "poller.h"
#include "event.h"
#include "thread.h"
#include "signal.h"
#include "worker.h"
#include "queue.h"

using std::vector;

namespace comm {

class EventWorker : public Event, public QueuedWorker, public Thread {
 public:
  EventWorker() {}
  virtual ~EventWorker() {}

  bool Init();
  void UnInit();

  bool Enqueue(Msg* msg);
  void DropAll();

 protected:
  int IoRead(int fd);
  int IoWrite(int fd);
  void RealExecute();

 protected:
  static const size_t kQueueCapacity = 1073741824;

  Poller loop_;
  Signaler signal_;

  Queue<Msg*, kQueueCapacity> mq_;
};

}

#endif
