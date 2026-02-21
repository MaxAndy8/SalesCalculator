#include "SqlAuthService.h"

#include "application/auth/AuthenticatedUser.h"
#include "infrastructure/db/DbConnectionProvider.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QUuid>
#include <QDebug>

namespace SC::Infrastructure::Auth
{

QStringList SqlAuthService::getActiveLogins()
{
    QStringList logins;
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();

    const char* sql =
        "SELECT description FROM users WHERE show = true AND NOT marked ORDER BY description";
    QSqlQuery query(db);

    if (!query.exec(sql))
    {
        qWarning() << "SqlAuthService::getActiveLogins:" << query.lastError().text();
        return logins;
    }

    while (query.next())
        logins.append(query.value(0).toString());

    return logins;
}

std::optional<SC::Application::Auth::AuthenticatedUser> SqlAuthService::authenticate(
    const QString& login,
    const QString& password)
{
    if (login.isEmpty())
        return std::nullopt;

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    query.prepare(
        "SELECT idrref, description, full_description, password "
        "FROM users "
        "WHERE description = :login AND show = true AND NOT marked "
        "LIMIT 1");
    query.bindValue(":login", login);

    if (!query.exec() || !query.next())
        return std::nullopt;

    QByteArray passwordStored = query.value("password").toByteArray();
    QByteArray passwordInput = password.toUtf8();
    if (passwordStored != passwordInput)
        return std::nullopt;

    QByteArray idrref = query.value("idrref").toByteArray();
    QString username = query.value("description").toString();
    QString displayName = query.value("full_description").toString();

    QUuid id;
    if (idrref.size() == 16)
        id = QUuid::fromRfc4122(idrref);

    QStringList roles;
    QSqlQuery rq(db);
    rq.prepare("SELECT role FROM user_roles WHERE user_idrref = :id");
    rq.bindValue(":id", idrref);
    if (rq.exec())
        while (rq.next())
            roles.append(rq.value("role").toString());

    return SC::Application::Auth::AuthenticatedUser(id, username, displayName, roles);
}

} // namespace SC::Infrastructure::Auth
