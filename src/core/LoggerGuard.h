#pragma once
#include "Logger.h"

namespace SC::Core
{




class LoggerGuard final
{
public:
    LoggerGuard() { Logger::initialize(); }
    ~LoggerGuard() { Logger::shutdown(); }

    // Забороняємо копіювання
    LoggerGuard(const LoggerGuard&) = delete;
    LoggerGuard& operator=(const LoggerGuard&) = delete;
};
}
