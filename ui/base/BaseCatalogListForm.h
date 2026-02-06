#pragma once
#include <QWidget>

namespace Ui {
class BaseCatalogListForm;
}

namespace SC::UI::Base {
class BaseCatalogListForm : public QWidget
{
    Q_OBJECT

public:
    explicit BaseCatalogListForm(QWidget *parent = nullptr);
    ~BaseCatalogListForm();

private:
    Ui::BaseCatalogListForm *ui;
};
} // namespace SC::UI::Base

