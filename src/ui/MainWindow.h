#pragma once

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
    std::unique_ptr<Ui::MainWindow> m_ui;
    SC::Application::Auth::AuthenticatedUser m_user;

    void setupUi();
};

} // namespace SC::UI
