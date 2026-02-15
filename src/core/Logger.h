#pragma once

#include <QString>

namespace SC::Core
{

class Logger final
{
public:
    static void initialize();
    static void shutdown();

    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);
};

} // namespace SC::Core
