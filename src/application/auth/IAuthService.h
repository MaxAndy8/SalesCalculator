#pragma once

#include "AuthenticatedUser.h"
#include <QStringList>
#include <optional>

namespace SC::Application::Auth
{

/// Порт: сервіс авторизації — список логінів для вибору та перевірка логін/пароль.
/// Одна сутність для обох операцій (одні й ті самі таблиці users, user_roles).
class IAuthService
{
public:
    virtual ~IAuthService() = default;

    /// Список логінів активних користувачів (для комбобоксу входу).
    virtual QStringList getActiveLogins() = 0;

    /// Перевірка логіна та пароля; повертає користувача при успіху.
    /// Пароль може бути порожнім (логін без пароля).
    virtual std::optional<AuthenticatedUser> authenticate(
        const QString& login,
        const QString& password) = 0;
};

} // namespace SC::Application::Auth
