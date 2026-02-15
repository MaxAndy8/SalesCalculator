#include <cstdlib>
#include <exception>

#include <QDialog>
#include <QDebug>

#include "ui/application/ScApplication.h"
#include "ui/MainWindow.h"

#include "ui/dialogs/SelectDatabaseDialog.h"
/*
#include "ui/dialogs/AuthDialog.h"

#include "core/AppContext.h"
#include "infrastructure/db/DbRuntimeGuard.h"
*/

int main(int argc, char *argv[])
{
    SC::UI::Application::ScApplication app(argc, argv);

    try
    {
        // ==============================
        // 1. Database selection
        // ==============================

        SC::UI::Dialogs::SelectDatabaseDialog dbDialog;
        if (dbDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;
/*
        auto dbInfo = dbDialog.selectedDatabase();
        SC::Core::AppContext::instance().setDatabase(dbInfo);

        // ==============================
        // 2. Database runtime
        // ==============================

        SC::Infrastructure::DB::DbRuntimeGuard dbGuard(dbInfo);

        // ==============================
        // 3. Authorization
        // ==============================

        SC::UI::Dialogs::AuthDialog authDialog;
        if (authDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;

        SC::Core::AppContext::instance().setCurrentUser(
            authDialog.currentLogin()
            );
        */
        // ==============================
        // 4. Main window
        // ==============================

        SC::UI::MainWindow mainWindow;
        mainWindow.show();

        return app.exec();
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Fatal error:" << ex.what();
        return EXIT_FAILURE;
    }
}
