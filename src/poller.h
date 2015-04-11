#ifndef _JFPU_POLLER_H_
#define _JFPU_POLLER_H_

#include <ext/hash_map>

#include "event.h"

using namespace __gnu_cxx;

namespace comm {

class Poller {
 public:
  typedef hash_map<int, Event*> EVENT_MAP;

  Poller();
  ~Poller();

  bool Init();
  void UnInit();

  void ProcessEvent();

  int AddRead(int sock, Event* io, bool mod = false);
  int AddWrite(int sock, Event* io, bool mod = false);
  int DelRead(int sock, Event* io);
  int DelWrite(int sock, Event* io);

  size_t Num() const {
    return events_.size();
  }

 private:
  void* wait_events_;
  int epfd_;
  int listen_max_;
  EVENT_MAP events_;
};

}

#endif
