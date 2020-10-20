/*************************************************************************
> File Name:  log_trival.h
> Author: shenming
> Created Time: Tue Oct 20 18:26:58 2020
************************************************************************/

#ifndef __LOG_TRIVAL_H__
#define __LOG_TRIVAL_H__

#define LOG_TRACE(...)  {                                           \
    fprintf(stderr,"[TRACE] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
}
#define LOG_DEBUG(...)  {                                           \
    fprintf(stderr,"[DEBUG] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
}
#define LOG_INFO(...)  {                                            \
    fprintf(stderr,"[INFO] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
}
#define LOG_WARN(...)  {                                          \
    fprintf(stderr,"[WARN] %s: Line %d: \t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                 \
    fprintf(stderr, "\n");                                        \
}

#define LOG_ERROR(...)  {                                          \
    fprintf(stderr,"[ERROR] %s: Line %d: \t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                  \
    fprintf(stderr, "\n");                                         \
}

#define LOG_CRITICAL(...)  {                                        \
    fprintf(stderr,"[ERROR] %s: Line %d: \t", __FILE__, __LINE__);  \
    fprintf(stderr, __VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                          \
    abort();                                                        \
}

#endif
