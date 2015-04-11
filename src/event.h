#ifndef _JFPU_EVENT_H_
#define _JFPU_EVENT_H_

#include <stdint.h>

namespace comm {

const unsigned int EV_TYPE_NONE = 0;
const unsigned int EV_TYPE_READ = 1;
const unsigned int EV_TYPE_WRITE = 2;
const unsigned int EV_TYPE_MAX = 4;

class Event {
 public:
  Event() : type_(EV_TYPE_NONE) {}
  virtual ~Event() {}

  virtual int IoRead(int fd) = 0;
  virtual int IoWrite(int fd) = 0;

  bool IsNoIO() {
    return (type_ == EV_TYPE_NONE) ? true : false;
  }

  bool IsRead() {
    return (type_ & EV_TYPE_READ) ? true : false;
  }

  bool IsWrite() {
    return (type_ & EV_TYPE_WRITE) ? true : false;
  }

  void SetRead() {
    type_ |= EV_TYPE_READ;
  }

  void SetWrite() {
    type_ |= EV_TYPE_WRITE;
  }

  void ResetRead() {
    type_ &=~EV_TYPE_READ;
  }

  void ResetWrite() {
    type_ &=~EV_TYPE_WRITE;
  }

  void SetRW() {
    type_ |= EV_TYPE_READ;
    type_ |= EV_TYPE_WRITE;
  }

  unsigned int GetType() const {
    return type_;
  }

 private:
  unsigned int type_;
};

}

#endif
