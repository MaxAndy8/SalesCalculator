#include "NomenclatureListForm.h"

#include "NomenclatureGroupForm.h"
#include "NomenclatureItemForm.h"
#include "NomenclatureTreeModel.h"
#include "application/catalogs/nomenclature/INomenclatureCommandService.h"
#include "core/Logger.h"
#include "ui/forms/base/FormSyncBus.h"
#include "ui/forms/base/MdiUtils.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSet>
#include <QTreeView>

#include <exception>
#include <vector>

namespace SC::UI::Forms::Catalogs::Nomenclature
{
namespace
{
constexpr auto kNomenclatureEntityKey = "catalog.nomenclature";

QString toHexId(const QByteArray& id)
{
    return QString::fromUtf8(id.toHex());
}
}

NomenclatureListForm::NomenclatureListForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
    SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService,
    SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService,
    QWidget* parent)
    : SC::UI::Forms::Base::ListForm(new NomenclatureTreeModel(queryService), parent),
      m_queryService(queryService),
      m_commandService(commandService),
      m_unitQueryService(unitQueryService),
      m_unitCommandService(unitCommandService)
{
    applyToolbarProfile(ToolbarProfile::CatalogGroupsAndItemsList);

    auto* model = static_cast<NomenclatureTreeModel*>(treeModel());
    connect(model, &NomenclatureTreeModel::fetchError, this,
            [this](const QString& message)
            {
                QMessageBox::warning(this, tr("Nomenclature load error"), message);
            });
    const auto applySearch = [this, model]()
    {
        if (model != nullptr && searchLineEdit() != nullptr)
            model->setSearchText(searchLineEdit()->text());
    };
    connect(searchLineEdit(), &QLineEdit::returnPressed, this, applySearch);
    connect(searchLineEdit(), &QLineEdit::editingFinished, this, applySearch);

    treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView()->setUniformRowHeights(true);
    treeView()->setRootIsDecorated(true);
    treeView()->setAlternatingRowColors(true);
    treeView()->header()->setStretchLastSection(false);
    treeView()->header()->setSectionResizeMode(QHeaderView::Interactive);

    connect(SC::UI::Forms::Base::FormSyncBus::instance(),
            &SC::UI::Forms::Base::FormSyncBus::entityChanged,
            this,
            [this](const QString& entityKey, const QByteArray& changedId)
            {
                if (entityKey == QString::fromUtf8(kNomenclatureEntityKey))
                    refreshPreserveSelection(changedId);
            });
}

void NomenclatureListForm::handleCreateItemRequested()
{
    if (m_commandService == nullptr)
        return;

    QByteArray parentId;
    if (treeView()->selectionModel() != nullptr)
    {
        const QModelIndex current = treeView()->selectionModel()->currentIndex();
        if (current.isValid())
            parentId = current.data(NomenclatureTreeModel::IdRole).toByteArray();
    }

    auto* form = new NomenclatureItemForm(m_queryService, m_commandService, m_unitQueryService, m_unitCommandService);
    form->startCreate(parentId);
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        form,
        QStringLiteral("Catalog_Nomenclature_ItemForm_Create"),
        tr("Nomenclature item"));
    SC::Core::Logger::info(QStringLiteral("Nomenclature item form opened for create."));
}

void NomenclatureListForm::handleCreateFolderRequested()
{
    if (m_commandService == nullptr)
        return;

    QByteArray parentId;
    if (treeView()->selectionModel() != nullptr)
    {
        const QModelIndex current = treeView()->selectionModel()->currentIndex();
        if (current.isValid())
            parentId = current.data(NomenclatureTreeModel::IdRole).toByteArray();
    }

    auto* form = new NomenclatureGroupForm(m_queryService, m_commandService);
    form->startCreate(parentId);
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        form,
        QStringLiteral("Catalog_Nomenclature_GroupForm_Create"),
        tr("Nomenclature group"));
    SC::Core::Logger::info(QStringLiteral("Nomenclature group form opened for create."));
}

void NomenclatureListForm::handleEditRequested()
{
    if (m_commandService == nullptr || treeView()->selectionModel() == nullptr)
        return;

    const QModelIndex current = treeView()->selectionModel()->currentIndex();
    if (!current.isValid())
        return;

    const QByteArray id = current.data(NomenclatureTreeModel::IdRole).toByteArray();
    const bool folder = current.data(NomenclatureTreeModel::FolderRole).toBool();
    if (id.isEmpty())
        return;

    if (folder)
    {
        auto* form = new NomenclatureGroupForm(m_queryService, m_commandService);
        form->startEdit(id);
        SC::UI::Forms::Base::MdiUtils::openFormInMdi(
            this,
            form,
            QStringLiteral("Catalog_Nomenclature_GroupForm_Edit_%1").arg(toHexId(id)),
            tr("Nomenclature group"));
        return;
    }

    auto* form = new NomenclatureItemForm(m_queryService, m_commandService, m_unitQueryService, m_unitCommandService);
    form->startEdit(id);
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        form,
        QStringLiteral("Catalog_Nomenclature_ItemForm_Edit_%1").arg(toHexId(id)),
        tr("Nomenclature item"));
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
        if (!result.affectedIds.empty())
            SC::UI::Forms::Base::FormSyncBus::instance()->notifyEntityChanged(
                QString::fromUtf8(kNomenclatureEntityKey));
    }
    catch (const std::exception& ex)
    {
        QMessageBox::warning(this, tr("Nomenclature delete mark error"), QString::fromUtf8(ex.what()));
    }
}

void NomenclatureListForm::handleRefreshRequested()
{
    refreshPreserveSelection();
}

void NomenclatureListForm::handleCopyRequested()
{
    if (m_commandService == nullptr || treeView()->selectionModel() == nullptr)
        return;

    const QModelIndex current = treeView()->selectionModel()->currentIndex();
    if (!current.isValid())
        return;

    const QByteArray id = current.data(NomenclatureTreeModel::IdRole).toByteArray();
    const bool folder = current.data(NomenclatureTreeModel::FolderRole).toBool();
    if (id.isEmpty())
        return;

    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto> loaded =
        m_commandService->fetchForEdit(id);
    if (!loaded.has_value())
        return;

    if (folder)
    {
        auto* form = new NomenclatureGroupForm(m_queryService, m_commandService);
        form->startCreateFromCopy(*loaded);
        SC::UI::Forms::Base::MdiUtils::openFormInMdi(
            this,
            form,
            QStringLiteral("Catalog_Nomenclature_GroupForm_Copy_%1").arg(toHexId(id)),
            tr("Nomenclature group (copy)"));
        SC::Core::Logger::info(QStringLiteral("Nomenclature group form opened for copy."));
        return;
    }

    auto* form = new NomenclatureItemForm(m_queryService, m_commandService, m_unitQueryService, m_unitCommandService);
    form->startCreateFromCopy(*loaded);
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        form,
        QStringLiteral("Catalog_Nomenclature_ItemForm_Copy_%1").arg(toHexId(id)),
        tr("Nomenclature item (copy)"));
    SC::Core::Logger::info(QStringLiteral("Nomenclature item form opened for copy."));
}

void NomenclatureListForm::refreshPreserveSelection(const QByteArray& preferredId)
{
    auto* model = static_cast<NomenclatureTreeModel*>(treeModel());
    if (model == nullptr || treeView()->selectionModel() == nullptr)
        return;

    QSet<QByteArray> selectedIdsSet;
    const QModelIndexList selectedRows = treeView()->selectionModel()->selectedRows(0);
    for (const QModelIndex& index : selectedRows)
    {
        const QByteArray id = index.data(NomenclatureTreeModel::IdRole).toByteArray();
        if (!id.isEmpty())
            selectedIdsSet.insert(id);
    }
    if (selectedIdsSet.isEmpty() && !preferredId.isEmpty())
        selectedIdsSet.insert(preferredId);

    std::vector<QByteArray> selectedIds;
    selectedIds.reserve(selectedIdsSet.size());
    for (const QByteArray& id : selectedIdsSet)
        selectedIds.push_back(id);

    model->refresh();

    if (selectedIds.empty())
        return;

    auto* selectionModel = treeView()->selectionModel();
    selectionModel->clearSelection();

    bool currentSet = false;
    for (const QByteArray& id : selectedIds)
    {
        const QModelIndex found = findIndexByIdWithFetch(id);
        if (!found.isValid())
            continue;

        selectionModel->select(found, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        if (!currentSet)
        {
            selectionModel->setCurrentIndex(found, QItemSelectionModel::NoUpdate);
            treeView()->scrollTo(found);
            currentSet = true;
        }
    }
}

QModelIndex NomenclatureListForm::findIndexByIdWithFetch(const QByteArray& id)
{
    if (id.isEmpty() || treeModel() == nullptr)
        return {};

    struct Frame
    {
        QModelIndex parent;
        int state = 0;
    };

    std::vector<Frame> stack;
    stack.push_back(Frame{QModelIndex(), 0});

    while (!stack.empty())
    {
        Frame frame = stack.back();
        stack.pop_back();

        while (treeModel()->canFetchMore(frame.parent))
            treeModel()->fetchMore(frame.parent);

        const int rows = treeModel()->rowCount(frame.parent);
        for (int row = 0; row < rows; ++row)
        {
            const QModelIndex index = treeModel()->index(row, 0, frame.parent);
            if (!index.isValid())
                continue;

            if (index.data(NomenclatureTreeModel::IdRole).toByteArray() == id)
                return index;

            stack.push_back(Frame{index, 0});
        }
    }

    return {};
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
