#ifndef _logmacro_H
#define _logmacro_H

#include <sys/types.h>
#include <string>

#include "wbl/logger.h"
#include "wbl/log_writer.h"

using std::string;

bool SysLogInit(const string& file, size_t level, const string& format, size_t size);
void SysLogResetPri(size_t level);
bool SoLogInit(const string& file, size_t level, const string& format, size_t size);
void SoLogResetPri(size_t level);

extern pid_t g_currentpid;
extern wbl::log::logger g_syslog;
extern wbl::log::logger g_solog;

#ifndef CodeFormat
  #define CodeFormat "%d, %s%d: %s, "
  #define CodeInfo g_currentpid, __FILE__ ":", __LINE__, __func__
#endif

#ifndef NewCodeFormat
#define NewCodeFormat "%d, %s%d: %s, tid:%u, "
#define NewCodeInfo g_currentpid, __FILE__ ":", __LINE__, __func__, pthread_self()
#endif

#define LogError(fmt, args...) \
  do { \
    if (g_syslog.get_priority() <= wbl::log::logger::eLogError) { \
      g_syslog.error(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define LogFatal(fmt, args...) \
  do { \
    if (g_syslog.get_priority() <= wbl::log::logger::eLogFatalError) { \
      g_syslog.fatal(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define LogWarn(fmt, args...) \
  do { \
    if (g_syslog.get_priority() <= wbl::log::logger::eLogWarning) { \
      g_syslog.warning(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define LogInfo(fmt, args...) \
  do { \
    if (g_syslog.get_priority() <= wbl::log::logger::eLogInfo) { \
      g_syslog.info(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define LogDebug(fmt, args...) \
  do { \
    if (g_syslog.get_priority() <= wbl::log::logger::eLogDebug) { \
      g_syslog.debug(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)


#define SoError(fmt, args...) \
  do { \
    if (g_solog.get_priority() <= wbl::log::logger::eLogError) { \
      g_solog.error(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define SoFatal(fmt, args...) \
  do { \
    if (g_solog.get_priority() <= wbl::log::logger::eLogFatalError) { \
      g_solog.fatal(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define SoWarn(fmt, args...) \
  do { \
    if (g_solog.get_priority() <= wbl::log::logger::eLogWarning) { \
      g_solog.fatal(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define SoInfo(fmt, args...) \
  do { \
    if (g_solog.get_priority() <= wbl::log::logger::eLogInfo) { \
      g_solog.info(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#define SoDebug(fmt, args...) \
  do { \
    if (g_solog.get_priority() <= wbl::log::logger::eLogDebug) { \
      g_solog.debug(NewCodeFormat fmt, NewCodeInfo, ##args); \
    } \
  } while (0)

#endif
