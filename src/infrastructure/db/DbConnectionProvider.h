#pragma once

#include <QtSql/QSqlDatabase>
#include <QString>
#include <memory>
#include <mutex>

#include "application/database/DatabaseConnectionInfo.h"

namespace SC::Infrastructure::DB
{

class DbConnectionProvider
{
public:
    // Ініціалізація після вибору БД у SelectDatabaseDialog
    static void initialize(const SC::Application::Database::DatabaseConnectionInfo& info);

    // Отримати з’єднання для поточного thread
    static QSqlDatabase current();

    // Закрити connection поточного thread
    static void closeCurrentThreadConnection();

    // Закрити всі connections (викликати при shutdown)
    static void shutdown();

private:
    static QSqlDatabase createConnection(const QString& name);

private:
    static std::unique_ptr<SC::Application::Database::DatabaseConnectionInfo> m_connectionInfo;
    static std::mutex m_mutex;
};

}
