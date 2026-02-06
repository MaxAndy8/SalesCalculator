#include "BaseCatalogListForm.h"
#include "ui_BaseCatalogListForm.h"

namespace SC::UI::Base {
BaseCatalogListForm::BaseCatalogListForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BaseCatalogListForm)
{
    ui->setupUi(this);

    // Отримуємо назву класу (наприклад, "SC::UI::Base::BaseCatalogListForm")
    const QString className = this->metaObject()->className();

    // Встановлюємо її як заголовок вікна
    setWindowTitle(className);

}

BaseCatalogListForm::~BaseCatalogListForm()
{
    delete ui;
}
} // namespace SC::UI::Base
