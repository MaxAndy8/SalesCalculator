#pragma once
#include "Logger.h"

namespace SC::Core
{

// Use LoggerGuard only in non-Qt entrypoints (CLI/tests/tools) where
// SC::UI::Application::ScApplication is not created and logger lifecycle
// must still be managed via RAII.
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
