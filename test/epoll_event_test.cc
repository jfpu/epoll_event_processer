#include <iostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <gtest/gtest.h>

#include "psudotime.h"
#include "msg.h"
#include "log.h"
#include "atomic.h"
#include "event_worker.h"

using std::cout;
using std::endl;
using namespace comm;

string path("log_test");
int level = 6;
int size = 10000000;

// 0:default, 1:usleep(1), 2:run(20us)
const int run_flag = 1;
const int MAX_NUM = 1000000;

void waste_cpu(bool bp = false) {
  Mutex mutex_;
  uint64_t max = 1000;

  uint64_t bt = 0;
  struct timeval stv;
  if (bp) {
    bt = CPsudoTime::getInstance()->getCurrentMSec();
    gettimeofday(&stv, NULL);
  }

  for (uint64_t ix = 0; ix < max; ++ix) {
    Guard guard(&mutex_);
  }

  if (bp) {
    uint64_t et = CPsudoTime::getInstance()->getCurrentMSec();
    uint64_t dms = et - bt;

    struct timeval etv;
    gettimeofday(&etv, NULL);
    uint64_t tdiff = (etv.tv_sec * 1000000 + etv.tv_usec) - (stv.tv_sec * 1000000 + stv.tv_usec);

    cout << "Time: " << max << ", time: " << dms << " ms" << endl;
    cout << "Time: " << max << ", time: " << tdiff << " us" << endl;
  }
}
class TestMsg : public Msg {
 public:
  static atomic_t num_;
  static uint64_t bt_;
  static uint64_t et_;

  void AddNum() {
    if (1 == run_flag)
      usleep(1);
    else if (2 == run_flag)
      waste_cpu();
    atomic_inc(&num_);

    if (1 == atomic_read(&num_)) {
      bt_ = CPsudoTime::getInstance()->getCurrentMSec();
    }

    if (MAX_NUM == atomic_read(&num_)) {
      et_ = CPsudoTime::getInstance()->getCurrentMSec();

      uint64_t dms = et_ - bt_;
      cout << "Performence: " << (int)MAX_NUM << ", flag: " << run_flag << ", time: " << dms << endl;
    }
  }
};

atomic_t TestMsg::num_ = ATOMIC_INIT(0);
uint64_t TestMsg::bt_ = 0;
uint64_t TestMsg::et_ = 0;

class TestWorker : public EventWorker {
 public:
  bool DoMsg(Msg* msg) {
    if (NULL == msg)
      return false;

    TestMsg* tmsg = dynamic_cast<TestMsg*>(msg);
    if (NULL == tmsg)
      return false;

    tmsg->AddNum();
    return true;
  }
};

class TestFrameWork : public testing::Test {
 public:
  TestWorker tw_;

  void SetUp() {
    if (!SysLogInit(path, level, "microsecond", size)) {
      cout << "slave init: init system log failed" << endl;
      return;
    }

    LogInfo("Test Start...");

    tw_.Init();
  }

  void TearDown() {
    tw_.UnInit();
    LogInfo("Test Stop...");
  }
  void PrintQueueNum() {
      cout << "TestWorker: " << ", " << tw_.QueueSize() << endl;
  }
};

TEST_F(TestFrameWork, Performance) {
  waste_cpu(true);
  for (int i = 0; i < MAX_NUM; ++i) {
    TestMsg* tmsg = new TestMsg;
    if (NULL == tmsg)
      return;

    tw_.Enqueue(tmsg);
  }

  while (MAX_NUM != atomic_read(&TestMsg::num_)) {
    // PrintQueueNum();
    usleep(1000);
  }
}

