#pragma once
#include <QWidget>

namespace Ui {
class BaseCatalogSelectForm;
}

namespace SC::UI::Base {
class BaseCatalogSelectForm : public QWidget
{
    Q_OBJECT

public:
    explicit BaseCatalogSelectForm(QWidget *parent = nullptr);
    ~BaseCatalogSelectForm();

private:
    Ui::BaseCatalogSelectForm *ui;
};
} // namespace SC::UI::Base
