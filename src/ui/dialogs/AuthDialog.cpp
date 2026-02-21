#include "AuthDialog.h"
#include "ui_AuthDialog.h"

#include "application/auth/IAuthService.h"
#include "application/auth/AuthenticatedUser.h"

#include <QMessageBox>

namespace SC::UI::Dialogs {

AuthDialog::AuthDialog(SC::Application::Auth::IAuthService* authService,
                       QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuthDialog)
    , m_authService(authService)
{
    ui->setupUi(this);

    ui->editPassword->setEchoMode(QLineEdit::Password);

    loadUsers();

    connect(ui->btnLogIn, &QPushButton::clicked, this, &AuthDialog::onLoginClicked);
}

AuthDialog::~AuthDialog()
{
    delete ui;
}

void AuthDialog::loadUsers()
{
    ui->comboxLogin->clear();

    if (!m_authService)
        return;

    const QStringList logins = m_authService->getActiveLogins();
    for (const QString& login : logins)
        ui->comboxLogin->addItem(login);
}

void AuthDialog::onLoginClicked()
{
    const QString login = ui->comboxLogin->currentText().trimmed();
    const QString password = ui->editPassword->text();

    if (login.isEmpty())
    {
        QMessageBox::warning(this, tr("Login"), tr("Enter login."));
        return;
    }
    if (!m_authService)
    {
        QMessageBox::critical(this, tr("Error"), tr("Auth service not available."));
        return;
    }

    auto result = m_authService->authenticate(login, password);
    if (result)
    {
        m_authenticatedUser = std::move(*result);
        accept();
    }
    else
    {
        QMessageBox::warning(this, tr("Login"), tr("Invalid login or password."));
    }
}

SC::Application::Auth::AuthenticatedUser AuthDialog::authenticatedUser() const
{
    return m_authenticatedUser.value();
}

} // namespace SC::UI::Dialogs
