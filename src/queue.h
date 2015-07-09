#ifndef _JFPU_QUEUE_H_
#define _JFPU_QUEUE_H_

#include <iostream>
#include <deque>

#include "mutex.h"

using std::cout;
using std::endl;
using std::deque;

namespace comm {

template<typename T, size_t MaxNumber>
class Queue {
 public:
  Queue() {}
  ~Queue() {}

  bool Push(T t) {
#if 0
    size_t qsize = q_.size();
    if (qsize >= MaxNumber) {
      cout << "Push Error: " << qsize << "Limit: " << MaxNumber << endl;
      return false;
    }
#endif

    Guard guard(&mutex_);
    q_.push_back(t);
    return true;
  }

  bool Pop(T& t) {
    Guard guard(&mutex_);

    if (q_.empty()) {
      return false;
    }

    t = q_.front();
    q_.pop_front();
    return true;
  }

  size_t TaskNum() {
    Guard guard(&mutex_);
    return q_.size();
  }

  bool Empty() const {
    Guard guard(&mutex_);
    return q_.empty();
  }

  void Clear() {
    q_.clear();
  }

  size_t MaxNum() {
    return MaxNumber;
  }

 private:
  Queue(const Queue& mq);
  const Queue& operator=(const Queue& mq);

 private:
  Mutex mutex_;
  deque<T> q_;
};

}

#endif
