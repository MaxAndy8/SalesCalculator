#include "NomenclatureTreeModel.h"

#include <QDebug>
#include <QIcon>
#include <exception>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

NomenclatureTreeModel::NomenclatureTreeModel(
    SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* queryService,
    QObject* parent,
    int pageSize)
    : QAbstractItemModel(parent),
      m_queryService(queryService),
      m_pageSize(pageSize > 0 ? pageSize : 150),
      m_root(std::make_unique<TreeNode>())
{
    m_root->isVirtualRoot = true;
    m_root->loadState = LoadState::NotLoaded;
}

QModelIndex NomenclatureTreeModel::index(int row, int column, const QModelIndex& parentIndex) const
{
    if (column < 0 || column >= columnCount() || row < 0)
        return {};

    TreeNode* parentNode = nodeFromIndex(parentIndex);
    if (parentNode == nullptr)
        return {};

    if (row >= static_cast<int>(parentNode->children.size()))
        return {};

    return createIndex(row, column, parentNode->children[static_cast<std::size_t>(row)].get());
}

QModelIndex NomenclatureTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    auto* childNode = static_cast<TreeNode*>(index.internalPointer());
    if (childNode == nullptr || childNode->parent == nullptr || childNode->parent->isVirtualRoot)
        return {};

    const int row = rowInParent(childNode->parent);
    if (row < 0)
        return {};

    return createIndex(row, 0, childNode->parent);
}

int NomenclatureTreeModel::rowCount(const QModelIndex& parentIndex) const
{
    if (parentIndex.column() > 0)
        return 0;

    TreeNode* parentNode = nodeFromIndex(parentIndex);
    if (parentNode == nullptr)
        return 0;

    return static_cast<int>(parentNode->children.size());
}

int NomenclatureTreeModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QVariant NomenclatureTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    switch (role)
    {
    case Qt::DisplayRole:
        return dataDisplay(index);
    case Qt::DecorationRole:
        return dataDecoration(index);
    default:
        return {};
    }
}

QVariant NomenclatureTreeModel::dataDisplay(const QModelIndex& index) const
{
    const auto* node = static_cast<const TreeNode*>(index.internalPointer());
    if (node == nullptr || node->isVirtualRoot)
        return {};

    switch (index.column())
    {
    case 0:
        return node->dto.code;
    case 1:
        return node->dto.name;
    case 2:
        if (node->dto.folder)
            return {};
        return node->dto.article;
    case 3:
        if (node->dto.folder)
            return {};
        return node->dto.unit;
    case 4:
        if (node->dto.folder)
            return {};
        return node->dto.service ? tr("Yes") : tr("No");
    default:
        return {};
    }
}

QVariant NomenclatureTreeModel::dataDecoration(const QModelIndex& index) const
{
    const auto* node = static_cast<const TreeNode*>(index.internalPointer());
    if (node == nullptr || node->isVirtualRoot || index.column() != 0)
        return {};

    if (node->dto.folder)
        return node->dto.marked ? QIcon(":/ForCatalogs/FolderDeletionMark16") : QIcon(":/ForCatalogs/Folder16");

    return node->dto.marked ? QIcon(":/ForCatalogs/ItemDeletionMark16") : QIcon(":/ForCatalogs/Item16");
}

QVariant NomenclatureTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Code");
        case 1:
            return tr("Name");
        case 2:
            return tr("Article");
        case 3:
            return tr("Unit");
        case 4:
            return tr("Service");
        default:
            return {};
        }
    }

    return {};
}

bool NomenclatureTreeModel::hasChildren(const QModelIndex& parentIndex) const
{
    if (!parentIndex.isValid())
        return true;

    const auto* node = static_cast<const TreeNode*>(parentIndex.internalPointer());
    if (node == nullptr)
        return false;

    return node->dto.hasChildren || !node->children.empty();
}

bool NomenclatureTreeModel::canFetchMore(const QModelIndex& parentIndex) const
{
    if (m_queryService == nullptr)
        return false;

    TreeNode* parentNode = nodeFromIndex(parentIndex);
    if (parentNode == nullptr)
        return false;

    if (parentNode->isVirtualRoot)
        return parentNode->loadState != LoadState::FullyLoaded;

    if (!parentNode->dto.hasChildren)
        return false;

    return parentNode->loadState != LoadState::FullyLoaded;
}

void NomenclatureTreeModel::fetchMore(const QModelIndex& parentIndex)
{
    if (!canFetchMore(parentIndex))
        return;

    TreeNode* parentNode = nodeFromIndex(parentIndex);
    if (parentNode == nullptr)
        return;

    appendPage(parentIndex, parentNode);
}

void NomenclatureTreeModel::refresh()
{
    beginResetModel();
    m_root->children.clear();
    m_root->loadState = LoadState::NotLoaded;
    m_root->nextCursor.reset();
    endResetModel();
    fetchMore(QModelIndex());
}

NomenclatureTreeModel::TreeNode* NomenclatureTreeModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return m_root.get();

    return static_cast<TreeNode*>(index.internalPointer());
}

QModelIndex NomenclatureTreeModel::indexFromNode(const TreeNode* node) const
{
    if (node == nullptr || node->isVirtualRoot || node->parent == nullptr)
        return {};

    const int row = rowInParent(node);
    if (row < 0)
        return {};

    return createIndex(row, 0, const_cast<TreeNode*>(node));
}

int NomenclatureTreeModel::rowInParent(const TreeNode* node) const
{
    if (node == nullptr || node->parent == nullptr)
        return -1;

    const auto& siblings = node->parent->children;
    for (std::size_t i = 0; i < siblings.size(); ++i)
    {
        if (siblings[i].get() == node)
            return static_cast<int>(i);
    }

    return -1;
}

void NomenclatureTreeModel::appendPage(const QModelIndex& parentIndex, TreeNode* parentNode)
{
    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage page;

    try
    {
        if (parentNode->isVirtualRoot)
        {
            page = m_queryService->fetchRootPage(m_pageSize, parentNode->nextCursor);
        }
        else
        {
            page = m_queryService->fetchChildrenPage(parentNode->dto.id, m_pageSize, parentNode->nextCursor);
        }
    }
    catch (const std::exception& ex)
    {
        qWarning() << "NomenclatureTreeModel fetch error:" << ex.what();
        emit fetchError(QString::fromUtf8(ex.what()));
        parentNode->loadState = LoadState::FullyLoaded;
        parentNode->nextCursor.reset();
        return;
    }

    if (page.items.empty())
    {
        parentNode->loadState = LoadState::FullyLoaded;
        parentNode->nextCursor.reset();
        return;
    }

    const int start = static_cast<int>(parentNode->children.size());
    const int end = start + static_cast<int>(page.items.size()) - 1;
    beginInsertRows(parentIndex, start, end);
    for (const auto& item : page.items)
    {
        auto child = std::make_unique<TreeNode>();
        child->dto = item;
        child->parent = parentNode;
        child->loadState = item.hasChildren ? LoadState::NotLoaded : LoadState::FullyLoaded;
        parentNode->children.push_back(std::move(child));
    }
    endInsertRows();

    parentNode->nextCursor = page.nextCursor;
    parentNode->loadState = parentNode->nextCursor.has_value()
        ? LoadState::PartiallyLoaded
        : LoadState::FullyLoaded;

    if (!parentNode->isVirtualRoot && parentNode->loadState == LoadState::FullyLoaded && parentNode->children.empty())
        parentNode->dto.hasChildren = false;

    const QModelIndex selfIndex = indexFromNode(parentNode);
    if (selfIndex.isValid())
        emit dataChanged(selfIndex, selfIndex);
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
