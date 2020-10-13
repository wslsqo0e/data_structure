/*************************************************************************
> File Name:  log_base.h
> Author: shenming
> Created Time: Tue Oct 13 15:11:19 2020
************************************************************************/

#ifndef __LOG_BASE_H__
#define __LOG_BASE_H__

#include <stdlib.h>
#include <iostream>

void AssertFailure_(const char *func, const char *file, int32_t line, const char *cond_str);

#ifndef NDEBUG
#define LOG_ASSERT(cond)                                    \
  do {                                                      \
    if (cond)                                               \
      (void)0;                                              \
    else                                                    \
      AssertFailure_(__func__, __FILE__, __LINE__, #cond);  \
  } while (0)
#else
#define LOG_ASSERT(cond) (void)0
#endif

#endif
