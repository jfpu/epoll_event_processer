#ifndef _JFPU_SIGNAL_H_
#define _JFPU_SIGNAL_H_

namespace comm {

class Signaler {
 public:
  Signaler();
  ~Signaler();

  int GetFd();
  bool Send();
  bool Recv();

 private:
  static bool SetUnBlock(int fd);
  static bool MakePair(int& rdFd, int& wrFd);

  int wrfd_;
  int rdfd_;
};

}

#endif
