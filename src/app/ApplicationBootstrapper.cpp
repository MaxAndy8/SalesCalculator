#include "ApplicationBootstrapper.h"

#include <QDialog>
#include <QMessageBox>

#include "core/LoggerGuard.h"

#include "ui/application/ScApplication.h"
#include "ui/dialogs/SelectDatabaseDialog.h"
#include "ui/dialogs/AuthDialog.h"
#include "ui/MainWindow.h"

#include "infrastructure/db/DbProviderGuard.h"
#include "infrastructure/auth/SqlAuthService.h"

namespace SC::App
{

ApplicationBootstrapper::ApplicationBootstrapper(int argc, char* argv[])
    : m_argc(argc)
    , m_argv(argv)
{
}

int ApplicationBootstrapper::run()
{
    // ==============================
    // 1. Logger lifecycle
    // ==============================
    SC::Core::LoggerGuard loggerGuard;

    SC::UI::Application::ScApplication app(m_argc, m_argv);

    try
    {
        // ==============================
        // 2. Database selection
        // ==============================
        SC::UI::Dialogs::SelectDatabaseDialog dbDialog;

        if (dbDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;

        auto dbInfo = dbDialog.selectedDatabase();

        // ==============================
        // 3. Infrastructure lifecycle
        // ==============================
        SC::Infrastructure::DB::DbProviderGuard dbGuard(dbInfo);

        // ==============================
        // 4. Authorization
        // ==============================
        SC::Infrastructure::Auth::SqlAuthService authService;
        SC::UI::Dialogs::AuthDialog authDialog(&authService);

        if (authDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;

        auto user = authDialog.authenticatedUser();
        // ==============================
        // 5. MainWindow
        // ==============================
        SC::UI::MainWindow mainWindow(user);
        mainWindow.show();

        return app.exec();
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Fatal error:" << ex.what();

        QMessageBox::critical(
            nullptr,
            "Fatal error",
            ex.what()
            );

        return EXIT_FAILURE;
    }
}


} // namespace SC::App
