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
#include "log_base.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"

#define level_trace spdlog::level::trace
#define level_debug spdlog::level::debug
#define level_info spdlog::level::info
#define level_warn spdlog::level::warn
#define level_error spdlog::level::error

#define LOG_SET_LEVEL(n) do {spdlog::set_level(n);} while (0)

#define LOG_TRACE SPDLOG_TRACE
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
// #define LOG_CRITICAL SPDLOG_CRITICAL
#define LOG_CRITICAL(...) do { SPDLOG_CRITICAL(__VA_ARGS__); abort(); } while (0)