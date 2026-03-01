#pragma once

#include "formController/IFormController.h"
#include <QModelIndex>
#include <QMainWindow>
#include "application/auth/AuthenticatedUser.h"
#include <memory>

namespace Ui
{
class MainWindow;
}

namespace SC::UI
{

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const SC::Application::Auth::AuthenticatedUser& user, QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;



private:
    Ui::MainWindow *ui;
    SC::Application::Auth::AuthenticatedUser m_user;
    SC::UI::IFormController* formController;

    void setupUi();

public slots:
    void openWindowForm(const QString idMdiSubWindow, const FormType formType, const QString WindowTitle = "", const QModelIndex index = QModelIndex() );

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    // Windows
    void on_actionCloseCurrentWindow_triggered();
    void on_actionCloseAllWindows_triggered();

    void on_actionWindowsTile_triggered();
    void on_actionWindowsCascade_triggered();

    void on_actionNextWindow_triggered();
    void on_actionPreviousWindow_triggered();

    // //////////////////////////////////////////////////////////////
    void on_actionCatalogsNomenclatureListForm_triggered();
};

} // namespace SC::UI
