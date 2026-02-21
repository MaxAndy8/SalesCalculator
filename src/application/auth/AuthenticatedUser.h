#pragma once

#include <QString>
#include <QStringList>
#include <QUuid>

namespace SC::Application::Auth
{

class AuthenticatedUser final
{
public:
    AuthenticatedUser(
        const QUuid& id,
        const QString& username,
        const QString& displayName,
        const QStringList& roles);

    // ===== Getters =====
    const QUuid& id() const noexcept;
    const QString& username() const noexcept;
    const QString& displayName() const noexcept;
    const QStringList& roles() const noexcept;

    // ===== Authorization helpers =====
    bool hasRole(const QString& role) const noexcept;
    bool isAdmin() const noexcept;

private:
    QUuid       m_id         ;
    QString     m_username   ;
    QString     m_displayName;
    QStringList m_roles      ;
};

} // namespace SC::Application::Auth
