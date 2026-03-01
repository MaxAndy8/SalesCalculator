#pragma once

#include "application/catalogs/nomenclature/INomenclatureTreeQueryService.h"

#include <QAbstractItemModel>

#include <memory>
#include <optional>
#include <vector>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureTreeModel final : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NomenclatureTreeModel(
        SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* queryService,
        QObject* parent = nullptr,
        int pageSize = 150);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

private:
    enum class LoadState
    {
        NotLoaded,
        PartiallyLoaded,
        FullyLoaded
    };

    struct TreeNode
    {
        SC::Application::Catalogs::Nomenclature::NomenclatureTreeNodeDto dto;
        TreeNode* parent = nullptr;
        std::vector<std::unique_ptr<TreeNode>> children;
        LoadState loadState = LoadState::NotLoaded;
        std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor> nextCursor;
        bool isVirtualRoot = false;
    };

private:
    TreeNode* nodeFromIndex(const QModelIndex& index) const;
    QModelIndex indexFromNode(const TreeNode* node) const;
    int rowInParent(const TreeNode* node) const;
    void appendPage(const QModelIndex& parentIndex, TreeNode* parentNode);

private:
    SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* m_queryService;
    int m_pageSize;
    std::unique_ptr<TreeNode> m_root;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
