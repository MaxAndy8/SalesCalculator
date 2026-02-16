#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QStatusBar>

namespace SC::UI
{

MainWindow::MainWindow(const SC::Application::Auth::AuthenticatedUser& user, QWidget* parent)
    : QMainWindow(parent),
      m_ui(std::make_unique<Ui::MainWindow>()),
      m_user(user)
{
    setupUi();

    // можна одразу застосувати ролі
    // setupUi();
    // applyPermissions(m_user);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    m_ui->setupUi(this);

    // Початковий стан статус-бару
    statusBar()->showMessage(tr("Ready"));
}

} // namespace SC::UI
