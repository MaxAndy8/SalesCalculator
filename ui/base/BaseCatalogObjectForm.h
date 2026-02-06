#pragma once
#include <QWidget>

namespace Ui {
class BaseCatalogObjectForm;
}

namespace SC::UI::Base {
class BaseCatalogObjectForm : public QWidget
{
    Q_OBJECT

public:
    explicit BaseCatalogObjectForm(QWidget *parent = nullptr);
    ~BaseCatalogObjectForm();

private:
    Ui::BaseCatalogObjectForm *ui;
};

} // namespace SC::UI::Base
