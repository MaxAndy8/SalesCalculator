#include "AuthenticatedUser.h"

namespace SC::Application::Auth
{

AuthenticatedUser::AuthenticatedUser(
    const QUuid& id,
    const QString& username,
    const QString& displayName,
    const QStringList& roles)
    : m_id(id)
    , m_username(username)
    , m_displayName(displayName)
    , m_roles(roles)
{
}

const QUuid& AuthenticatedUser::id() const noexcept
{
    return m_id;
}

const QString& AuthenticatedUser::username() const noexcept
{
    return m_username;
}

const QString& AuthenticatedUser::displayName() const noexcept
{
    return m_displayName;
}

const QStringList& AuthenticatedUser::roles() const noexcept
{
    return m_roles;
}

bool AuthenticatedUser::hasRole(const QString& role) const noexcept
{
    return m_roles.contains(role, Qt::CaseInsensitive);
}

bool AuthenticatedUser::isAdmin() const noexcept
{
    return hasRole("admin");
}

} // namespace SC::Application::Auth
