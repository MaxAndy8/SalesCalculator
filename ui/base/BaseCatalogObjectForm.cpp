#include "BaseCatalogObjectForm.h"
#include "ui_BaseCatalogObjectForm.h"

namespace SC::UI::Base {
BaseCatalogObjectForm::BaseCatalogObjectForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BaseCatalogObjectForm)
{
    ui->setupUi(this);

    // Отримуємо назву класу (наприклад, "SC::UI::Base::BaseCatalogListForm")
    const QString className = this->metaObject()->className();

    // Встановлюємо її як заголовок вікна
    setWindowTitle(className);


}

BaseCatalogObjectForm::~BaseCatalogObjectForm()
{
    delete ui;
}
} // namespace SC::UI::Base
