#include "SqlNomenclatureQueryService.h"

#include "infrastructure/db/DbConnectionProvider.h"
#include "infrastructure/db/UnitOfWork.h"

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
        qWarning() << "SqlNomenclatureQueryService::executePagedQuery:" << error;
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

SC::UI::FormType SqlNomenclatureQueryService::formType() const
{
    return SC::UI::FormType::Catalog_Nomenclature_ListForm;
}

bool SqlNomenclatureQueryService::CreateItem()
{
    throw std::logic_error("CreateItem is not implemented");
}

bool SqlNomenclatureQueryService::CreateFolder()
{
    throw std::logic_error("CreateFolder is not implemented");
}

bool SqlNomenclatureQueryService::Write(const QByteArray& id)
{
    (void)id;
    throw std::logic_error("Write is not implemented");
}

bool SqlNomenclatureQueryService::Delete(const QByteArray& id)
{
    (void)id;
    throw std::logic_error("Delete is not implemented");
}

SC::Application::Catalogs::Nomenclature::ToggleDeletionMarkResult
SqlNomenclatureQueryService::ToggleDeletionMarkForSelection(const std::vector<QByteArray>& selectedIds)
{
    SC::Application::Catalogs::Nomenclature::ToggleDeletionMarkResult result;
    if (selectedIds.empty())
        return result;

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();

    QString seedSql;
    seedSql.reserve(48 * static_cast<int>(selectedIds.size()));
    for (int i = 0; i < static_cast<int>(selectedIds.size()); ++i)
    {
        if (i > 0)
            seedSql += QStringLiteral(" UNION ALL ");

        seedSql += QStringLiteral("SELECT CAST(:id%1 AS bytea) AS id").arg(i);
    }

    QString sql = QStringLiteral(
        "WITH RECURSIVE seed(id) AS ( %1 ), "
        "affected(id) AS ( "
        "    SELECT id FROM seed "
        "    UNION "
        "    SELECT n.idrref "
        "    FROM nomenclature n "
        "    JOIN affected a ON n.parent_idrref = a.id "
        "    WHERE n.idrref <> a.id "
        "), "
        "target(value) AS ( "
        "    SELECT CASE "
        "        WHEN EXISTS ("
        "            SELECT 1 "
        "            FROM nomenclature n "
        "            JOIN affected a ON a.id = n.idrref "
        "            WHERE n.marked = TRUE"
        "        ) THEN FALSE "
        "        ELSE TRUE "
        "    END "
        ") "
        "UPDATE nomenclature n "
        "SET marked = (SELECT value FROM target) "
        "WHERE n.idrref IN (SELECT id FROM affected) "
        "RETURNING n.idrref, n.marked")
                      .arg(seedSql);

    QSqlQuery query(db);
    query.prepare(sql);
    for (int i = 0; i < static_cast<int>(selectedIds.size()); ++i)
        query.bindValue(QStringLiteral(":id%1").arg(i), selectedIds[static_cast<std::size_t>(i)]);

    try
    {
        SC::Infrastructure::Persistence::UnitOfWork::begin();
        if (!query.exec())
        {
            const QString error = query.lastError().text();
            qWarning() << "SqlNomenclatureQueryService::ToggleDeletionMarkForSelection:" << error;
            throw std::runtime_error(error.toStdString());
        }

        bool markedInitialized = false;
        while (query.next())
        {
            result.affectedIds.push_back(query.value(0).toByteArray());
            if (!markedInitialized)
            {
                result.newMarkedValue = query.value(1).toBool();
                markedInitialized = true;
            }
        }

        qInfo() << "SqlNomenclatureQueryService::ToggleDeletionMarkForSelection"
                << "selectedIds:" << selectedIds.size()
                << "affectedIds:" << result.affectedIds.size()
                << "newMarkedValue:" << result.newMarkedValue;

        SC::Infrastructure::Persistence::UnitOfWork::commit();
        return result;
    }
    catch (...)
    {
        SC::Infrastructure::Persistence::UnitOfWork::rollback();
        throw;
    }
}

SC::Application::Catalogs::Nomenclature::NomenclatureTreePage
SqlNomenclatureQueryService::fetchRootPage(
    int limit,
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor)
{
    return executePagedQuery(std::nullopt, limit, cursor);
}

SC::Application::Catalogs::Nomenclature::NomenclatureTreePage
SqlNomenclatureQueryService::fetchChildrenPage(
    const QByteArray& parentId,
    int limit,
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor)
{
    return executePagedQuery(parentId, limit, cursor);
}

} // namespace SC::Infrastructure::Catalogs::Nomenclature
