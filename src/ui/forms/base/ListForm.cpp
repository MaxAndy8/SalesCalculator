#include "ListForm.h"
#include "ui_ListForm.h"

#include <QAbstractItemModel>

namespace SC::UI::Forms::Base
{

ListForm::ListForm(QAbstractItemModel* model, QWidget* parent)
    : QWidget(parent),
      m_model(model),
      ui(new Ui::ListForm)
{
    ui->setupUi(this);
    if (m_model != nullptr && m_model->parent() == nullptr)
        m_model->setParent(this);

    ui->treeView->setModel(m_model);
}

ListForm::~ListForm()
{
    delete ui;
}

QTreeView* ListForm::treeView() const
{
    return ui->treeView;
}

void ListForm::setTreeModel(QAbstractItemModel* model)
{
    m_model = model;
    ui->treeView->setModel(m_model);
}

QAbstractItemModel* ListForm::treeModel() const
{
    return m_model;
}

} // namespace SC::UI::Forms::Base
