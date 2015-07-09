#ifndef _QUEUE_LOCKFREE_
#define _QUEUE_LOCKFREE_

#include <iostream>

using std::cout;
using std::endl;

namespace comm {


template<typename T>
class Node {
 public:
  T value_;
  Node* next_;

  Node() : next_(NULL) {}
  Node(T& t, Node* ptr = NULL) : value_(t), next_(ptr) {}
};

template<typename T, size_t MaxNumber>
class Queue {
 public:
  Queue() : head_(NULL), tail_(NULL) {
    head_ = new Node<T>();
    tail_ = head_;
  }

  ~Queue() {
    Clear();
  }

  bool Push(T& t) {
    Node<T>* nd = new Node<T>(t);
    if (NULL == nd)
      return false;

    bool ret = false;
    Node<T>* tt = NULL;
    do {
      tt = tail_;
      ret = __sync_bool_compare_and_swap(&(tt->next_), NULL, nd);
      if (!ret)
        __sync_bool_compare_and_swap(&tail_, tt, tt->next_);
    } while (!ret);
    __sync_bool_compare_and_swap(&tail_, tt, nd);

    return true;
  }

  bool Pop(T& t) {
    Node<T>* th = NULL;
    do {
      th = head_;
      if (NULL == th->next_)
        return false;

    } while (!__sync_bool_compare_and_swap(&head_, th, th->next_));

    t = th->next_->value_;
    delete th;

    return true;
  }

  size_t TaskNum() {
    size_t num = 0;
    Node<T>* tp = head_;
    while (NULL != tp) {
      ++num;
      tp = tp->next_;
    }

    return --num;
  }

  bool Empty() const {
    if (head_ == tail_)
      return true;
    return false;
  }

  void Clear() {
    Node<T>* tp = head_;
    while (tail_ != tp) {
      Node<T>* tmp = tp;
      tp = tp->next_;
      delete [] tmp;
    }
  }

  size_t MaxNum() {
    return MaxNumber;
  }

 private:
  Queue(const Queue& mq);
  const Queue& operator=(const Queue& mq);

 private:
   Node<T>* head_;
   Node<T>* tail_;
};

}

#endif
