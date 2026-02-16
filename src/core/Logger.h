#pragma once

#include <QString>

namespace SC::Core
{

/**
 * @brief використання паттерну RAII (Resource Acquisition Is Initialization)
 *
 * Створіть спеціальний клас-обгортку, який буде викликати initialize()
 * у конструкторі та shutdown() у деструкторі.
 */

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
