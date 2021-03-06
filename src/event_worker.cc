#include <stdlib.h>
#include <pthread.h>

#include "log.h"
#include "event_worker.h"

namespace comm {

static const size_t kMaxMsgNumPer = 4;

bool EventWorker::Init() {
  int fd = signal_.GetFd();
  if (!loop_.Init() || 0 != loop_.AddRead(fd, this)) {
    LogError(" failed, fd: %d", fd);
    return false;
  }

  LogInfo("worker, start sock: 0x%x", fd);

  Run();

  return true;
}

void EventWorker::UnInit() {
  DropAll();
  usleep(1000);

  LogInfo("worker exit, left msg=%d", mq_.TaskNum());

  SetStopping();
  Stop();
  // WaitStop();
  loop_.UnInit();
}

bool EventWorker::Enqueue(Msg* msg) {
  if (!msg) {
    return false;
  }

  if (!mq_.Push(msg)) {
    LogError("enqueue tasker failed");
    return false;
  }

  if (!signal_.Send()) {
    LogError("send to signal failed");
    return false;
  }

  LogInfo("msg: 0x%x", (long int)msg);
  return true;
}

void EventWorker::DropAll() {
  while (mq_.TaskNum() > 0) {
    Msg* msg = NULL;

    if (mq_.Pop(msg)) {
      DropMsg(msg);
    } else {
      break;
    }
  }
}

size_t EventWorker::QueueSize() {
  return mq_.TaskNum();
}

int EventWorker::IoRead(int fd) {
  if (fd != signal_.GetFd() || !signal_.Recv()) {
    LogError("read from signal failed");
    return 0;
  }

  Msg* msg = NULL;
  while (mq_.Pop(msg)) {
    if (NULL == msg) {
      // LogError("pop empty msg: fd: %d", fd);
      return 0;
    }

    LogInfo("fd: %d, msg: 0x%x", fd, (long int)msg);
    DoMsg(msg);
    DropMsg(msg);
  }

  return 0;
}

int EventWorker::IoWrite(int fd) {
  (void)fd;
  return 0;
}

void EventWorker::RealExecute() {
  while (Running()) {
    loop_.ProcessEvent();
    pthread_testcancel();
  }
}

}
