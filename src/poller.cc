#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "log.h"
#include "poller.h"
#include "event.h"

namespace comm {

const size_t kEpollMaxFd = 1024;
const int kHashBucket = 1000;
const int kMaxFdReady = 1000;
const size_t kTimeOut = 3;

Poller::Poller() : epfd_(-1), listen_max_(1), events_(kHashBucket) {
}

Poller::~Poller() {
}

bool Poller::Init() {
  struct rlimit limit;

  if (-1 == getrlimit(RLIMIT_NOFILE, &limit)) {
    LogError("EPOLL call getrlimit failed");
    return false;
  }

  listen_max_ = (int)((kEpollMaxFd > limit.rlim_cur) ? limit.rlim_cur : kEpollMaxFd);
  epfd_ = epoll_create(listen_max_);
  if (-1 == epfd_) {
    switch (errno) {
      case ENOMEM:
        LogError("call epoll_create failed");
        break;

      case ENFILE:
        LogError("exceed max file descriptor");
        break;

      case EINVAL:
        LogError("input param is error");
        break;

      default:
        break;
    }

    return false;
  }

  wait_events_ = new char[sizeof(struct epoll_event) * listen_max_];
  if (NULL == wait_events_) {
    return false;
  }

  return true;
}

void Poller::UnInit() {
  if (NULL != wait_events_) {
    delete [] wait_events_;
  }
}

int Poller::AddRead(int sock, Event* io, bool mod) {
  if (NULL == io || sock <= 0) {
    return -1;
  }

  io->SetRead();
  events_[sock] = io;

  struct epoll_event ev = {0, {0}};
  ev.data.fd = sock;
  ev.events |= EPOLLIN; // | EPOLLET;

  unsigned int op = (!mod) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

  LogInfo(" sock: 0x%x, op: 0x%x", sock, op);

  return epoll_ctl(epfd_, op, sock, &ev);
}

int Poller::AddWrite(int sock, Event* io, bool mod) {
  if (NULL == io || sock <= 0) {
    return -1;
  }

  io->SetWrite();
  events_[sock] = io;

  struct epoll_event ev = {0, {0}};
  ev.data.fd = sock;
  ev.events |= EPOLLOUT;  // | EPOLLET;

  unsigned int op = (!mod) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

  LogInfo(" sock: 0x%x, op: 0x%x", sock, op);

  return epoll_ctl(epfd_, op, sock, &ev);
}

int Poller::DelRead(int sock, Event* io) {
  if (NULL == io || sock <= 0) {
    return -1;
  }

  Event* oldio = events_[sock];
  if (oldio != io) {
    return -1;
  }

  io->ResetRead();
  if (io->IsNoIO()) {
    events_[sock] = NULL;
  }

  struct epoll_event ev = {0, {0}};
  ev.data.fd = sock;
  ev.events |= EPOLLIN; // | EPOLLET;

  LogInfo("pop read sock: 0x%x", sock);

  return epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, &ev);
}

int Poller::DelWrite(int sock, Event* io) {
  if (NULL == io || sock <= 0) {
    return -1;
  }

  Event* oldio = events_[sock];
  if (oldio != io) {
    return -1;
  }

  io->ResetWrite();
  if (io->IsNoIO()) {
    events_[sock] = NULL;
  }

  struct epoll_event ev = {0, {0}};
  ev.data.fd = sock;
  ev.events |= EPOLLOUT;  //  | EPOLLET;

  LogInfo("pop write sock: 0x%x", sock);

  return epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, &ev);
}

void Poller::ProcessEvent() {
  LogInfo("start again...");

  struct timeval tv;
  gettimeofday(&tv, NULL);
  int timeout = (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000);

  int ret = epoll_wait(epfd_, (struct epoll_event*)wait_events_, listen_max_, timeout);
  if (ret <= 0) {
    return;
  }

  struct epoll_event* tmpev = NULL;
  for (int i = 0; i < ret; i++) {
    tmpev = (struct epoll_event*)wait_events_ + i;

    int sock = tmpev->data.fd;
    Event* io = events_[sock];
    if (NULL == io) {
      LogError("event find fail: sock: 0x%x", sock);
      continue;
    }

    if ((tmpev->events & EV_TYPE_READ)
        && io->IsRead() && 0 != io->IoRead(sock)) {
      LogError("excute read callback failed: sock: 0x%x", sock);
    }

    if ((tmpev->events & EV_TYPE_WRITE)
        && io->IsWrite() && 0 != io->IoWrite(sock)) {
      LogError("excute write callback failed: sock: 0x%x", sock);
    }
  }
}

}
