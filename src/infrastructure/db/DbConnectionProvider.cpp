#include "DbConnectionProvider.h"

#include <QThread>

#include <QtSql/QSqlError>
#include <QUuid>
#include <QDebug>

namespace SC::Infrastructure::DB
{

std::unique_ptr<SC::Application::Database::DatabaseConnectionInfo>
    DbConnectionProvider::m_connectionInfo = nullptr;

std::mutex DbConnectionProvider::m_mutex;

void DbConnectionProvider::initialize(const SC::Application::Database::DatabaseConnectionInfo& info)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connectionInfo = std::make_unique<SC::Application::Database::DatabaseConnectionInfo>(info);
}

QSqlDatabase DbConnectionProvider::current()
{
    thread_local QString connectionName;

    if (connectionName.isEmpty())
    {
        connectionName = QString("sc_connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

        QSqlDatabase db = createConnection(connectionName);

        if (!db.open())
        {
            qCritical() << "DB connection error:"
                        << db.lastError().text();
            throw std::runtime_error("Database connection failed");
        }
    }

    return QSqlDatabase::database(connectionName);
}

QSqlDatabase DbConnectionProvider::createConnection(const QString& name)
{
    if (!m_connectionInfo)
        throw std::runtime_error("DbConnectionProvider not initialized");

    const auto& info = *m_connectionInfo;

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", name);

    db.setHostName(    info.host    );
    db.setPort(        info.port    );
    db.setDatabaseName(info.database);
    db.setUserName(    info.user    );
    db.setPassword(    info.password);

    if (!db.open())
    {
        qCritical() << "DB connection error:"
                    << db.lastError().text();
        throw std::runtime_error("Database connection failed");
    }

    return db;
}

void DbConnectionProvider::closeCurrentThreadConnection()
{
    thread_local QString connectionName;

    if (!connectionName.isEmpty())
    {
        QSqlDatabase db =
            QSqlDatabase::database(connectionName);

        if (db.isOpen())
            db.close();

        QSqlDatabase::removeDatabase(connectionName);
        connectionName.clear();
    }
}

void DbConnectionProvider::shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const auto connections = QSqlDatabase::connectionNames();

    for (const auto& name : connections)
    {
        QSqlDatabase db = QSqlDatabase::database(name);
        if (db.isOpen())
            db.close();

        QSqlDatabase::removeDatabase(name);
    }

    m_connectionInfo.reset();
}

}
