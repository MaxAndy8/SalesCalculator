#pragma once

#include <QString>

namespace SC::Application::Database
{

/**
 * @brief DTO конфігурації підключення до сховища даних
 *
 * Не містить логіки підключення.
 * Не залежить від Infrastructure.
 * Використовується UI та Infrastructure.
 */
struct DatabaseConnectionInfo
{
    QString name;      ///< Qt connection name
    QString host;      ///< Server host
    int     port = 0;  ///< Server port
    QString database;  ///< Database name
    QString user;      ///< Username
    QString password;  ///< Password
};

} // namespace SC::Application::Database
