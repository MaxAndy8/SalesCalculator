#pragma once

#include "application/catalogs/ICatalogQueryService.h"
#include "application/forms/ReferenceFieldPolicy.h"

#include <QByteArray>
#include <QString>
#include <QVector>

#include <optional>
#include <vector>

namespace SC::Application::Catalogs::Nomenclature
{

struct NomenclatureTreeCursor
{
    bool lastFolder = false;
    QString lastCode;
    QByteArray lastId;
};

struct NomenclatureTreeNodeDto
{
    QByteArray id;
    std::optional<QByteArray> parentId;
    bool folder = false;
    bool marked = false;
    QString code;
    QString article;
    QString unit;
    bool service = false;
    QString name;
    bool hasChildren = false;
};

struct NomenclatureTreePage
{
    std::vector<NomenclatureTreeNodeDto> items;
    std::optional<NomenclatureTreeCursor> nextCursor;
};

struct ToggleDeletionMarkResult
{
    std::vector<QByteArray> affectedIds;
    bool newMarkedValue = false;
};

class INomenclatureQueryService : public SC::Application::Catalogs::ICatalogQueryService
{
public:
    virtual ~INomenclatureQueryService() = default;

    virtual ToggleDeletionMarkResult ToggleDeletionMarkForSelection(
        const std::vector<QByteArray>& selectedIds) = 0;

    virtual NomenclatureTreePage fetchRootPage(
        int limit,
        const std::optional<NomenclatureTreeCursor>& cursor,
        const QString& searchText = QString()) = 0;

    virtual NomenclatureTreePage fetchChildrenPage(
        const QByteArray& parentId,
        int limit,
        const std::optional<NomenclatureTreeCursor>& cursor,
        const QString& searchText = QString()) = 0;

    /// Пошук для автозаповнення: LIKE по description, code, article, barcode; фільтр за folder залежно від allowedKinds.
    virtual QVector<SC::Application::Forms::AutocompleteEntry> searchForAutocomplete(
        const QString& searchText,
        SC::Application::Forms::AllowedNodeKinds allowedKinds,
        int limit) = 0;
};

} // namespace SC::Application::Catalogs::Nomenclature
