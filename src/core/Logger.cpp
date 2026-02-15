#include "Logger.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>

namespace
{
QFile g_logFile;
QMutex g_mutex;
}

namespace SC::Core
{

void Logger::initialize()
{
    g_logFile.setFileName("SalesCalculator.log");

    if (!g_logFile.open(QIODevice::Append | QIODevice::Text))
    {
        qWarning() << "Failed to open log file";
    }
}

void Logger::shutdown()
{
    g_logFile.close();
}

static void write(const QString& level, const QString& message)
{
    QMutexLocker locker(&g_mutex);

    QTextStream out(&g_logFile);
    out << QDateTime::currentDateTime().toString(Qt::ISODate)
        << " [" << level << "] "
        << message << "\n";
    out.flush();
}

void Logger::info(const QString& message)
{
    write("INFO", message);
}

void Logger::warning(const QString& message)
{
    write("WARN", message);
}

void Logger::error(const QString& message)
{
    write("ERROR", message);
}

} // namespace SC::Core
