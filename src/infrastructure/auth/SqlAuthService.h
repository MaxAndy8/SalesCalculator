#pragma once

#include "application/auth/IAuthService.h"

namespace SC::Infrastructure::Auth
{

/// Реалізація IAuthService через БД (users, user_roles).
/// Паролі у відкритому вигляді; підтримується логін з порожнім паролем.
class SqlAuthService final : public SC::Application::Auth::IAuthService
{
public:
    QStringList getActiveLogins() override;
    std::optional<SC::Application::Auth::AuthenticatedUser> authenticate(
        const QString& login,
        const QString& password) override;
};

} // namespace SC::Infrastructure::Auth
