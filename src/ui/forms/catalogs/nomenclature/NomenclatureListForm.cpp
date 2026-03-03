#include "NomenclatureListForm.h"

#include "NomenclatureTreeModel.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSet>
#include <QTreeView>

#include <exception>
#include <vector>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

NomenclatureListForm::NomenclatureListForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
    QWidget* parent)
    : SC::UI::Forms::Base::ListForm(new NomenclatureTreeModel(queryService), parent),
      m_queryService(queryService)
{
    auto* model = static_cast<NomenclatureTreeModel*>(treeModel());
    connect(model, &NomenclatureTreeModel::fetchError, this,
            [this](const QString& message)
            {
                QMessageBox::warning(this, tr("Nomenclature load error"), message);
            });

    treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView()->setUniformRowHeights(true);
    treeView()->setRootIsDecorated(true);
    treeView()->setAlternatingRowColors(true);
    treeView()->header()->setStretchLastSection(false);
    treeView()->header()->setSectionResizeMode(QHeaderView::Interactive);
}

void NomenclatureListForm::handleDeleteRequested()
{
    if (m_queryService == nullptr || treeView()->selectionModel() == nullptr)
        return;

    const QModelIndexList selectedRows = treeView()->selectionModel()->selectedRows(0);
    if (selectedRows.isEmpty())
        return;

    QSet<QByteArray> selectedIdsSet;
    selectedIdsSet.reserve(selectedRows.size());
    for (const QModelIndex& index : selectedRows)
    {
        const QByteArray id = index.data(NomenclatureTreeModel::IdRole).toByteArray();
        if (!id.isEmpty())
            selectedIdsSet.insert(id);
    }

    if (selectedIdsSet.isEmpty())
        return;

    std::vector<QByteArray> selectedIds;
    selectedIds.reserve(selectedIdsSet.size());
    for (const QByteArray& id : selectedIdsSet)
        selectedIds.push_back(id);

    try
    {
        const auto result = m_queryService->ToggleDeletionMarkForSelection(selectedIds);
        static_cast<NomenclatureTreeModel*>(treeModel())->applyMarkedState(
            result.affectedIds, result.newMarkedValue);
    }
    catch (const std::exception& ex)
    {
        QMessageBox::warning(this, tr("Nomenclature delete mark error"), QString::fromUtf8(ex.what()));
    }
}

void NomenclatureListForm::handleRefreshRequested()
{
    auto* model = static_cast<NomenclatureTreeModel*>(treeModel());
    if (model != nullptr)
        model->refresh();
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
