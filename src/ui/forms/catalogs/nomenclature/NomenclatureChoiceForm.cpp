#include "NomenclatureChoiceForm.h"

#include "NomenclatureTreeModel.h"
#include "core/Logger.h"

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QMdiSubWindow>
#include <QAbstractItemModel>
#include <QTreeView>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

namespace
{
using AllowedNodeKinds = SC::Application::Forms::AllowedNodeKinds;

bool isAllowed(const AllowedNodeKinds allowedKinds, const bool folder)
{
    switch (allowedKinds)
    {
    case AllowedNodeKinds::ItemsOnly:
        return !folder;
    case AllowedNodeKinds::FoldersOnly:
        return folder;
    case AllowedNodeKinds::ItemsAndFolders:
    default:
        return true;
    }
}
} // namespace

NomenclatureChoiceForm::NomenclatureChoiceForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
    AllowedNodeKinds allowedKinds,
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
    SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService,
    QWidget* parent)
    : NomenclatureListForm(queryService, commandService, unitQueryService, nullptr, parent),
      m_allowedKinds(allowedKinds)
{
    setWindowTitle(tr("Nomenclature choice"));
    treeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    if (allowedKinds == AllowedNodeKinds::FoldersOnly)
        applyToolbarProfile(ToolbarProfile::CatalogGroupsAndItemsChoiceGroup);
    else
        applyToolbarProfile(ToolbarProfile::CatalogGroupsAndItemsChoice);

    // Confirm choice only on explicit activation via double click or Enter.
    connect(treeView(), &QTreeView::doubleClicked, this, [this](const QModelIndex&)
            { tryAcceptCurrentSelection(); });
    connect(treeView(), &QTreeView::activated, this, [this](const QModelIndex&)
            { tryAcceptCurrentSelection(); });

    if (m_allowedKinds == AllowedNodeKinds::FoldersOnly)
    {
        if (treeView()->model() != nullptr)
        {
            connect(treeView()->model(), &QAbstractItemModel::rowsInserted, this,
                    [this](const QModelIndex&, int, int)
                    {
                        hideDisallowedRows();
                    });
            connect(treeView()->model(), &QAbstractItemModel::modelReset, this,
                    [this]()
                    {
                        hideDisallowedRows();
                    });
        }
        connect(treeView(), &QTreeView::expanded, this, [this](const QModelIndex&)
                { hideDisallowedRows(); });
        hideDisallowedRows();
    }
}

void NomenclatureChoiceForm::tryAcceptCurrentSelection()
{
    if (treeView()->selectionModel() == nullptr)
        return;

    const QModelIndex current = treeView()->selectionModel()->currentIndex();
    if (!current.isValid())
        return;

    const bool folder = current.data(NomenclatureTreeModel::FolderRole).toBool();
    if (!isAllowed(m_allowedKinds, folder))
    {
        SC::Core::Logger::info(QStringLiteral("Choice ignored: node kind disallowed in nomenclature form."));
        return;
    }

    const QByteArray id = current.data(NomenclatureTreeModel::IdRole).toByteArray();
    if (id.isEmpty())
        return;

    const QString display = current.data(NomenclatureTreeModel::NameRole).toString().trimmed();
    emit selectionAccepted(
        id,
        display,
        folder ? AllowedNodeKinds::FoldersOnly : AllowedNodeKinds::ItemsOnly);

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(parentWidget()))
    {
        mdiSubWindow->close();
        return;
    }

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(window()))
    {
        mdiSubWindow->close();
        return;
    }

    close();
}

void NomenclatureChoiceForm::hideDisallowedRows()
{
    if (m_allowedKinds != AllowedNodeKinds::FoldersOnly || treeView()->model() == nullptr)
        return;

    applyRowVisibility(QModelIndex());
}

void NomenclatureChoiceForm::applyRowVisibility(const QModelIndex& parentIndex)
{
    if (treeView()->model() == nullptr)
        return;

    const int rows = treeView()->model()->rowCount(parentIndex);
    for (int row = 0; row < rows; ++row)
    {
        const QModelIndex index = treeView()->model()->index(row, 0, parentIndex);
        if (!index.isValid())
            continue;

        const bool folder = index.data(NomenclatureTreeModel::FolderRole).toBool();
        treeView()->setRowHidden(row, parentIndex, !folder);
        applyRowVisibility(index);
    }
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
