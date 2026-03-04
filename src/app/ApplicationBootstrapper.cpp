#include "ApplicationBootstrapper.h"

#include <QDialog>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QThread>

#include "ui/application/ScApplication.h"
#include "ui/dialogs/SelectDatabaseDialog.h"
#include "ui/dialogs/AuthDialog.h"
#include "ui/MainWindow.h"
#include "FormControllerImpl.h"

#include "infrastructure/db/DbProviderGuard.h"
#include "infrastructure/auth/SqlAuthService.h"

#include "infrastructure/catalogs/nomenclature/SqlNomenclatureQueryService.h"

namespace SC::App
{

ApplicationBootstrapper::ApplicationBootstrapper(int argc, char* argv[])
    : m_argc(argc)
    , m_argv(argv)
{
}

int ApplicationBootstrapper::run()
{
    SC::UI::Application::ScApplication app(m_argc, m_argv);

    try
    {
        // ==============================
        // 1. Database selection
        // ==============================
        SC::UI::Dialogs::SelectDatabaseDialog dbDialog;

        if (dbDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;

        auto dbInfo = dbDialog.selectedDatabase();

        // ==============================
        // 2. Infrastructure lifecycle
        // ==============================
        SC::Infrastructure::DB::DbProviderGuard dbGuard(dbInfo);

        // ==============================
        // 3. Authorization
        // ==============================
        SC::Infrastructure::Auth::SqlAuthService authService;
        SC::UI::Dialogs::AuthDialog authDialog(&authService);

        if (authDialog.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;

        auto user = authDialog.authenticatedUser();
        // ==============================
        // 4. Form controller + MainWindow
        // ==============================
        SC::App::FormControllerImpl formController(user);

        formController.addSqlQueryService(
            new SC::Infrastructure::Catalogs::Nomenclature::SqlNomenclatureQueryService()); // для FormControllerImpl::getForm(SC::UI::FormType::Catalog_Nomenclature_ListForm)

        //formController.addSqlQueryService(new SC::Infrastructure::Catalogs::Users::SqlUsersTreeQueryService());                 // для FormControllerImpl::getForm(SC::UI::FormType::Catalog_Users_ListForm)
        //formController.addSqlQueryService(new SC::Infrastructure::Catalogs::Units::SqlUnitsTreeQueryService());                 // для FormControllerImpl::getForm(SC::UI::FormType::Catalog_Units_ListForm)
        //formController.addSqlQueryService(new SC::Infrastructure::Document::GoodsReceipt::SqlGoodsReceiptTreeQueryService());   // для FormControllerImpl::getForm(SC::UI::FormType::Document_GoodsReceipt_ListForm)
        //formController.addSqlQueryService(new SC::Infrastructure::Document::Pricings::SqlPricingsTreeQueryService());           // // для FormControllerImpl::getForm(SC::UI::FormType::Document_GoodsReceipt_ListForm)

        // Створюємо піксельну карту для заставки
        QPixmap pix(":/SC_App/SplashScreen"); // Шлях до вашого зображення
        QSplashScreen splash(pix);

        // Показуємо заставку
        splash.show();

        // Можна виводити текст поверх зображення
        splash.showMessage("Завантаження модулів...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);

        // Емуляція тривалого завантаження
        // В реальному проекті тут ініціалізуються бази даних або мережа
        for (int i = 0; i <= 100; i += 20) {
            splash.showMessage(QString("Завантаження: %1%").arg(i),
                               Qt::AlignBottom | Qt::AlignCenter, Qt::white);
            QThread::msleep(500); // Затримка для демонстрації
            app.processEvents();    // Оновлюємо інтерфейс, щоб заставка не "зависла"
        }

        // Створюємо та показуємо головне вікно
        SC::UI::MainWindow mainWindow(user, &formController);
        // Закриваємо заставку після появи головного вікна
        splash.finish(&mainWindow);
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
