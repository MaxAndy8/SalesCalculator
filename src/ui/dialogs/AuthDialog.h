#pragma once
#include <QDialog>
#include <optional>

#include "application/auth/AuthenticatedUser.h"

namespace Ui {
class AuthDialog;
}

namespace SC::Application::Auth {
class IAuthService;
}

namespace SC::UI::Dialogs {

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(SC::Application::Auth::IAuthService* authService,
                        QWidget *parent = nullptr);
    virtual  ~AuthDialog();

    SC::Application::Auth::AuthenticatedUser authenticatedUser() const;

private slots:
    void onLoginClicked();

private:
    void loadUsers();

private:
    Ui::AuthDialog *ui;
    SC::Application::Auth::IAuthService* m_authService = nullptr;
    std::optional<SC::Application::Auth::AuthenticatedUser> m_authenticatedUser;
};

} // namespace SC::UI::Dialogs
