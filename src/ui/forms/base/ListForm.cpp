#include "ListForm.h"
#include "ui_ListForm.h"

namespace SC::UI::Forms::Base
{

ListForm::ListForm(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ListForm)
{
    ui->setupUi(this);
}

ListForm::~ListForm()
{
    delete ui;
}

} // namespace SC::UI::Forms::Base
