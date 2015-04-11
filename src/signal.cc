#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef EVENT_SUP
#include <sys/eventfd.h>
#endif

#include "log.h"
#include "signal.h"

#define CHECK_TRUE(PARM) \
do \
{ \
  if (!(PARM)) { \
    LogError("assert failed"); \
  } \
} while(0);

namespace comm {

Signaler::Signaler() : wrfd_(-1), rdfd_(-1) {
#ifdef EVENTFD_SUP
  CHECK_TRUE(MakePair(rdfd_, wrfd_) && SetUnBlock(rdfd_) && SetUnBlock(wrfd_));
#else
  CHECK_TRUE(MakePair(rdfd_, wrfd_));
#endif
}

Signaler::~Signaler() {
  if (-1 != wrfd_) {
    close(wrfd_);
  }

  if (-1 != rdfd_) {
    close(rdfd_);
  }
}

int Signaler::GetFd() {
  return rdfd_;
}

bool Signaler::Send() {
#ifdef EVENTFD_SUP
  uint64_t wto = 1;
  ssize_t rc = write(wrfd_, &wto, sizeof(wto));
  if (sizeof(wto) != rc && errno != EAGAIN) {
    LogError("send err, rc=%lld", rc);
    return false;
  }
#else
  u_char notify = 1;
  ssize_t rc = write(wrfd_, &notify, sizeof(u_char));
  if (sizeof(u_char) != rc && errno != EAGAIN) {
    LogError("send err, rc=%lld", rc);
    return false;
  }
#endif
  return true;
}

bool Signaler::Recv() {
#ifdef EVENTFD_SUP
  uint64_t rfrom = 0;
  ssize_t rc = read(rdfd_, &rfrom, sizeof (rfrom));
  if (rc == -1 && errno != EAGAIN) {
    LogError("recv err, rc=%lld", rc);
    return false;
  }
#else
  uint32_t rfrom = 0;
  ssize_t rc = read(rdfd_, &rfrom, sizeof(uint32_t));
  if (-1 == rc && errno != EAGAIN) {
    LogError("recv err, rc=%lld", rc);
    return false;
  }
#endif
  return true;
}

bool Signaler::MakePair(int& rfd, int& wfd) {
#ifdef EVENTFD_SUP
  int fd = eventfd(0, 0);
  if (-1 == fd) {
    return false;
  }

  wfd = fd;
  rfd = fd;
#else
  int fd[2] = {0};
  if (-1 == pipe(fd)) {
    return false;
  }

  wfd = fd[1];
  rfd = fd[0];
#endif
  return true;
}

bool Signaler::SetUnBlock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    flags = 0;
  }

  return (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) ? false : true;
}

}
