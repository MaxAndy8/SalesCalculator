#pragma once

#include <QString>

namespace SC::Core::DB {

/**
 * @brief Дані підключення до БД
 *
 * Зберігає лише конфігурацію.
 * Логіки підключення тут НЕМАЄ.
 */
struct DatabaseConnectionInfo {
    QString name    ;
    QString host    ;
    int     port    ;
    QString database;
    QString user    ;
    QString password;
};

} // namespace SC::Core::DB
