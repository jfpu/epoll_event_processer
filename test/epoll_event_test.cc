#include <iostream>
#include <string>
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

const int MAX_NUM = 10000000;

class TestMsg : public Msg {
 public:
  static atomic_t num_;
  static uint64_t bt_;
  static uint64_t et_;

  void AddNum() {
    atomic_inc(&num_);

    if (1 == atomic_read(&num_)) {
      bt_ = CPsudoTime::getInstance()->getCurrentMSec();
    }

    if (MAX_NUM == atomic_read(&num_)) {
      et_ = CPsudoTime::getInstance()->getCurrentMSec();

      uint64_t dms = et_ - bt_;
      cout << "Performence: " << (int)MAX_NUM << " , time: " << dms << endl;
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
};

TEST_F(TestFrameWork, Performance) {
  for (int i = 0; i < MAX_NUM; ++i) {
    TestMsg* tmsg = new TestMsg;
    if (NULL == tmsg)
      return;

    tw_.Enqueue(tmsg);
  }

  while (MAX_NUM != atomic_read(&TestMsg::num_)) {
    usleep(1000);
  }

}

