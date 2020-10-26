/*************************************************************************
> File Name:  log_trival.h
> Author: shenming
> Created Time: Tue Oct 20 18:26:58 2020
************************************************************************/

#ifndef __LOG_TRIVAL_H__
#define __LOG_TRIVAL_H__

#define level_trace 16
#define level_debug 8
#define level_info 4
#define level_warn 2
#define level_error 1

#define LOG_SET_LEVEL(n) do {g_log_level = n} while (0)

extern int g_log_level;

#define LOG_TRACE(...)  do {                                        \
    if (g_log_level < level_trace) break;                           \
    fprintf(stderr,"[TRACE] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
  } while (0)
#define LOG_DEBUG(...)  {                                           \
    if (g_log_level < level_debug) break;                           \
    fprintf(stderr,"[DEBUG] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
  } while (0)
#define LOG_INFO(...)  {                                          \
    if (g_log_level < level_info) break;                          \
    fprintf(stderr,"[INFO] %s: Line %d: \t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                 \
    fprintf(stderr, "\n");                                        \
  } while (0)
#define LOG_WARN(...)  {                                          \
    if (g_log_level < level_warn) break;                          \
    fprintf(stderr,"[WARN] %s: Line %d: \t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                 \
    fprintf(stderr, "\n");                                        \
  } while (0)

#define LOG_ERROR(...)  {                                           \
    if (g_log_level < level_error) break;                           \
    fprintf(stderr,"[ERROR] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
  } while (0)

#define LOG_CRITICAL(...)  {                                        \
    fprintf(stderr,"[ERROR] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
    abort();                                                        \
  }

#endif
