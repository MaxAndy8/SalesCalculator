#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QStatusBar>

namespace SC::UI
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_ui(std::make_unique<Ui::MainWindow>())
{
    setupUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    m_ui->setupUi(this);

    // Початковий стан статус-бару
    statusBar()->showMessage(tr("Ready"));
}

} // namespace SC::UI
