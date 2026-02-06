#include "BaseCatalogSelectForm.h"
#include "ui_BaseCatalogSelectForm.h"


namespace SC::UI::Base {
BaseCatalogSelectForm::BaseCatalogSelectForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BaseCatalogSelectForm)
{
    ui->setupUi(this);

    // Отримуємо назву класу (наприклад, "SC::UI::Base::BaseCatalogListForm")
    const QString className = this->metaObject()->className();

    // Встановлюємо її як заголовок вікна
    setWindowTitle(className);

}

BaseCatalogSelectForm::~BaseCatalogSelectForm()
{
    delete ui;
}
} // namespace SC::UI::Base
