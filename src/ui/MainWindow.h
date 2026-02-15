#pragma once

#include <QMainWindow>
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
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

    void setupUi();
};

} // namespace SC::UI
