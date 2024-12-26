#pragma once

#ifndef LOGGING_H
#define LOGGING_H

#include "sokol_log.h"


#if defined(_DEBUG)
    #define LOG(logLevel, msg) slog_func(nullptr, logLevel, 0, msg, __LINE__, __FILE__, nullptr)
    #define DEBUG_LOG(msg) slog_func(nullptr, LOG_DEBUG, 0, msg, __LINE__, __FILE__, nullptr)
#else
    #define LOG(logLevel, msg) slog_func(nullptr, logLevel, 0, msg, __LINE__, nullptr, nullptr)
    #define DEBUG_LOG(msg) 
#endif

enum LogLevel{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
};

#endif // LOGGING_H