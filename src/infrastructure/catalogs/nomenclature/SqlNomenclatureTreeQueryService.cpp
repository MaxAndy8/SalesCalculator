#include "SqlNomenclatureTreeQueryService.h"

#include "infrastructure/db/DbConnectionProvider.h"

#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <algorithm>
#include <stdexcept>

namespace SC::Infrastructure::Catalogs::Nomenclature
{
namespace
{
using Cursor = SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor;
using NodeDto = SC::Application::Catalogs::Nomenclature::NomenclatureTreeNodeDto;
using Page = SC::Application::Catalogs::Nomenclature::NomenclatureTreePage;

constexpr int kDefaultLimit = 150;
constexpr int kMaxLimit = 500;

int normalizeLimit(int limit)
{
    if (limit <= 0)
        return kDefaultLimit;

    return std::min(limit, kMaxLimit);
}

QString baseSelectSql()
{
    return QStringLiteral(
        "SELECT n.idrref, "
        "       n.parent_idrref, "
        "       n.folder, "
        "       n.marked, "
        "       n.code, "
        "       n.article, "
        "       COALESCE(u.description, '') AS unit_description, "
        "       n.service, "
        "       n.description, "
        "       EXISTS ("
        "           SELECT 1 "
        "           FROM nomenclature child "
        "           WHERE child.parent_idrref = n.idrref "
        "             AND child.idrref <> n.idrref"
        "       ) AS has_children "
        "FROM nomenclature n "
        "LEFT JOIN units u ON u.idrref = n.unit_idrref ");
}

void bindCursor(QSqlQuery& query, const std::optional<Cursor>& cursor)
{
    if (!cursor.has_value())
        return;

    query.bindValue(":cursor_folder", cursor->lastFolder);
    query.bindValue(":cursor_code", cursor->lastCode);
    query.bindValue(":cursor_id", cursor->lastId);
}

void appendCursorPredicate(QString& sql, const std::optional<Cursor>& cursor)
{
    if (!cursor.has_value())
        return;

    sql += QStringLiteral(
        " AND ("
        "    (n.folder = :cursor_folder AND (n.code > :cursor_code OR (n.code = :cursor_code AND n.idrref > :cursor_id)))"
        "    OR "
        "    (n.folder = false AND :cursor_folder = true)"
        " ) ");
}

Page executePagedQuery(
    const std::optional<QByteArray>& parentId,
    int limit,
    const std::optional<Cursor>& cursor)
{
    const int normalizedLimit = normalizeLimit(limit);
    const int fetchLimit = normalizedLimit + 1;

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    QString sql = baseSelectSql();
    if (parentId.has_value())
        sql += QStringLiteral(
            "WHERE n.parent_idrref = :parent_id "
            "  AND n.idrref <> :parent_id ");
    else
        sql += QStringLiteral(
            "WHERE n.parent_idrref IS NULL "
            "   OR n.parent_idrref = n.idrref ");

    appendCursorPredicate(sql, cursor);
    sql += QStringLiteral(
        "ORDER BY n.folder DESC, n.code ASC, n.idrref ASC "
        "LIMIT :limit");

    query.prepare(sql);
    if (parentId.has_value())
        query.bindValue(":parent_id", *parentId);
    bindCursor(query, cursor);
    query.bindValue(":limit", fetchLimit);

    Page page;
    if (!query.exec())
    {
        const QString error = query.lastError().text();
        qWarning() << "SqlNomenclatureTreeQueryService::executePagedQuery:" << error;
        throw std::runtime_error(error.toStdString());
    }

    while (query.next())
    {
        NodeDto dto;
        dto.id = query.value(0).toByteArray();
        const QVariant parentValue = query.value(1);
        if (!parentValue.isNull())
            dto.parentId = parentValue.toByteArray();
        dto.folder = query.value(2).toBool();
        dto.marked = query.value(3).toBool();
        dto.code = query.value(4).toString();
        dto.article = query.value(5).toString();
        dto.unit = query.value(6).toString();
        dto.service = query.value(7).toBool();
        dto.name = query.value(8).toString();
        dto.hasChildren = query.value(9).toBool();
        page.items.push_back(std::move(dto));
    }

    if (static_cast<int>(page.items.size()) > normalizedLimit)
    {
        const NodeDto lastLoaded = page.items.at(normalizedLimit - 1);
        page.nextCursor = Cursor{
            .lastFolder = lastLoaded.folder,
            .lastCode = lastLoaded.code,
            .lastId = lastLoaded.id
        };
        page.items.resize(normalizedLimit);
    }

    return page;
}
} // namespace

SC::UI::FormType SqlNomenclatureTreeQueryService::formType() const
{
    return SC::UI::FormType::Catalog_Nomenclature_ListForm;
}

SC::Application::Catalogs::Nomenclature::NomenclatureTreePage
SqlNomenclatureTreeQueryService::fetchRootPage(
    int limit,
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor)
{
    return executePagedQuery(std::nullopt, limit, cursor);
}

SC::Application::Catalogs::Nomenclature::NomenclatureTreePage
SqlNomenclatureTreeQueryService::fetchChildrenPage(
    const QByteArray& parentId,
    int limit,
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor)
{
    return executePagedQuery(parentId, limit, cursor);
}

} // namespace SC::Infrastructure::Catalogs::Nomenclature
