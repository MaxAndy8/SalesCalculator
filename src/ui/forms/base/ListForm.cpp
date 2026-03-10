#include "ListForm.h"
#include "ui_ListForm.h"

#include <QAbstractItemModel>
#include <QAction>

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

QLineEdit* ListForm::searchLineEdit() const
{
    return ui->searchLineEdit;
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

void ListForm::applyToolbarProfile(const ToolbarProfile profile)
{
    const auto setCreateItemVisible = [this](bool visible)
    {
        ui->createItemButton->setVisible(visible);
        ui->actionCreateItem->setVisible(visible);
    };
    const auto setCreateFolderVisible = [this](bool visible)
    {
        ui->createFolderButton->setVisible(visible);
        ui->actionCreateFolder->setVisible(visible);
    };
    const auto setCreateText = [this](const QString& text)
    {
        ui->createItemButton->setText(text);
        ui->actionCreateItem->setText(text);
    };

    // defaults
    setCreateItemVisible(true);
    setCreateFolderVisible(true);
    setCreateText(tr("Create item"));

    switch (profile)
    {
    case ToolbarProfile::CatalogGroupsAndItemsList:
    case ToolbarProfile::CatalogGroupsAndItemsChoice:
        setCreateItemVisible(true);
        setCreateFolderVisible(true);
        setCreateText(tr("Create item"));
        break;
    case ToolbarProfile::CatalogGroupsAndItemsChoiceGroup:
        setCreateItemVisible(false);
        setCreateFolderVisible(true);
        break;
    case ToolbarProfile::CatalogItemsOnlyList:
    case ToolbarProfile::CatalogItemsOnlyChoice:
        setCreateItemVisible(true);
        setCreateFolderVisible(false);
        setCreateText(tr("Create item"));
        break;
    case ToolbarProfile::DocumentList:
    case ToolbarProfile::DocumentChoice:
        setCreateItemVisible(true);
        setCreateFolderVisible(false);
        setCreateText(tr("Create"));
        break;
    default:
        break;
    }
}

void ListForm::handleCreateItemRequested()
{
    //***********************************************
}

void ListForm::handleCreateFolderRequested()
{
    //***********************************************
}

void ListForm::handleEditRequested()
{
    //***********************************************
}

void ListForm::handleDeleteRequested()
{
    //***********************************************
}

void ListForm::handleRefreshRequested()
{
    //***********************************************
}

void ListForm::handleCopyRequested()
{
}

void ListForm::on_actionCreateItem_triggered()
{
    handleCreateItemRequested();
}

void ListForm::on_actionCreateFolder_triggered()
{
    handleCreateFolderRequested();
}

void ListForm::on_actionCopy_triggered()
{
    handleCopyRequested();
}

void ListForm::on_actionEdit_triggered()
{
    handleEditRequested();
}

void ListForm::on_actionDelete_triggered()
{
    handleDeleteRequested();
}

void ListForm::on_actionRefresh_triggered()
{
    handleRefreshRequested();
}











} // namespace SC::UI::Forms::Base
