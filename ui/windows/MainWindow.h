#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace SC::UI {

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

} // namespace SC::UI
