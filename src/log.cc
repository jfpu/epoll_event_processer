#include "log.h"

wbl::log::async_writer_thread so_awt;
wbl::log::async_writer_thread sys_awt;

pid_t g_currentpid;
wbl::log::logger g_syslog;
wbl::log::logger g_solog;

bool SysLogInit(const string& file,
                  size_t level,
                  const string& format, size_t size) {
  bool ret = g_syslog.initialize(level,
                                 sys_awt.async_file(file + ".log",
                                                    wbl::log::cut_by_size_and_day(size)),
                                                    format.c_str());
  if (ret) {
    return sys_awt.start();
  }

  return false;
}

void SysLogReInit(size_t level) {
  g_syslog.set_priority((wbl::log::logger::ELogPriority)level);
}

bool SoLogInit(const string& file,
                 size_t level,
                 const string& format, size_t size) {
  bool ret = g_solog.initialize(level,
                                so_awt.async_file(file + ".log",
                                                  wbl::log::cut_by_day(size)),
                                                  format.c_str());
  if (ret) {
    return so_awt.start();
  }

  return true;
}

void SoLogReInit(size_t level) {
  g_solog.set_priority((wbl::log::logger::ELogPriority)level);
}



