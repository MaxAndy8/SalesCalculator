#include <QMessageBox>
#include <QMdiSubWindow>
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QStatusBar>

namespace SC::UI
{

MainWindow::MainWindow(const SC::Application::Auth::AuthenticatedUser& user,
                       IFormController* formController,
                       QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_user(user),
      formController(formController)
{
    setupUi();

    // можна одразу застосувати ролі
    // setupUi();
    // applyPermissions(m_user);
}

MainWindow::~MainWindow()
{
    ui->mdiArea->closeAllSubWindows();
    delete ui;
}

void MainWindow::setupUi()
{
    ui->setupUi(this);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // Початковий стан статус-бару
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::openWindowForm(const QString idMdiSubWindow, const FormType formType, const QString WindowTitle, const QModelIndex index)
{
        bool needNewForm = true;
        QMdiSubWindow* MdiSubWindow;

        foreach(QMdiSubWindow* window, ui->mdiArea->subWindowList())
        {
            QString idMdiSubWin = window->property("idMdiSubWindow").toString();
            if (idMdiSubWin == idMdiSubWindow) {
                needNewForm  = false  ;
                MdiSubWindow = window ;
            }
        }

        if(needNewForm)
        {
            QWidget *modelWork = formController->getForm(formType);

            modelWork->setAttribute(Qt::WA_DeleteOnClose);
            modelWork->setWindowTitle(WindowTitle);

            QMdiSubWindow* newForm = ui->mdiArea->addSubWindow(modelWork);
            newForm->setProperty("idMdiSubWindow", idMdiSubWindow);

            newForm->show();
        }
        else {
            ui->mdiArea->setActiveSubWindow(MdiSubWindow);
        }

}

void MainWindow::on_actionExit_triggered()
{
    ui->mdiArea->closeAllSubWindows();
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About RS"),
                       tr("<HTML><HEAD>"
                          "<META content=""text/html; charset=utf-8"" http-equiv=Content-Type>"
                          "<META name=GENERATOR content=""MSHTML 8.00.7601.17514""></HEAD>"
                          "<BODY>"
                          "<P>"
                          "<FONT size=4>""Sales calculator"", release 0.1. <BR></FONT>"
                          "Configuration development: ""programmer-site.top"", 2008-2026"
                          "<BR>&nbsp;(0.1.0.0) ("
                          "<A href=""http://programmer-site.top/RS/egionalSolutions_UA_UTP.htm"">"
                          "http://programmer-site.top/RS/egionalSolutions_UA_UTP.htm</A>)"
                          "<BR>Copyright © ""programmer-site.top"", 2008-2026<BR> ("
                          "<A href=""http://programmer-site.top"">http://programmer-site.top</A>)"
                          "</P>"
                          "</BODY>"
                          "</HTML>"));
}

void MainWindow::on_actionCloseCurrentWindow_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actionCloseAllWindows_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionWindowsTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionWindowsCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionNextWindow_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionPreviousWindow_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionCatalogsNomenclatureListForm_triggered()
{
    QString  idWindow    = "CatalogNomenclatureListForm";
    FormType formType    = FormType::Catalog_Nomenclature_ListForm;
    QString  windowTitle = tr("Catalog nomenclature");
    openWindowForm(idWindow, formType, windowTitle);
}

void MainWindow::on_actionCatalogsUnitsListForm_triggered()
{
    QString  idWindow    = "CatalogsUsersListForm";
    FormType formType    = FormType::Catalog_Users_ListForm;
    QString  windowTitle = tr("Catalogs users");
    openWindowForm(idWindow, formType, windowTitle);
}



} // namespace SC::UI
