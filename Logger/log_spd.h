/*************************************************************************
> File Name:  log_spd.h
> Author: shenming
> Created Time: Fri Oct  9 23:22:39 2020
> 参考 https://github.com/gabime/spdlog

> LOGLEVEL:
> SPDLOG_LEVEL_TRACE
> SPDLOG_LEVEL_DEBUG
> SPDLOG_LEVEL_INFO
> SPDLOG_LEVEL_WARN
> SPDLOG_LEVEL_ERROR
> SPDLOG_LEVEL_CRITICAL
************************************************************************/
#include <stdlib.h>
// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_TRACE_ON
#include "spdlog/include/spdlog/spdlog.h"
using namespace spdlog;

#define LOG_TRACE SPDLOG_TRACE
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
// #define LOG_CRITICAL SPDLOG_CRITICAL
#define LOG_CRITICAL(...) do { SPDLOG_CRITICAL(__VA_ARGS__); abort(); } while (0)
