#include "application/ScApplication.h"
#include "windows/MainWindow.h"
#include "dialogs/SelectDatabaseDialog.h"
/*
#include "ui/authdialog.h"


using namespace SC;
*/
int main(int argc, char *argv[])
{
    SC::UI::Application::ScApplication app(argc, argv);

    SC::UI::Dialogs::SelectDatabaseDialog dbDialog;
    if (dbDialog.exec() != QDialog::Accepted)
        return 0;
/*
    UI::AuthDialog authDialog;
    if (authDialog.exec() != QDialog::Accepted)
        return 0;
*/
    SC::UI::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
