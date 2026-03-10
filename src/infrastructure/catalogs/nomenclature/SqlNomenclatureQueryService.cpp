#include "SqlNomenclatureQueryService.h"

#include "core/AutoNumbering.h"
#include "infrastructure/db/DbConnectionProvider.h"
#include "infrastructure/db/UnitOfWork.h"

#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QUuid>

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
/** Advisory lock key for generating unique nomenclature codes (prevents duplicate codes under concurrent writes). */
constexpr qint64 kAdvisoryLockKeyNomenclatureCode = 100001;

void acquireAdvisoryLockForNomenclatureCode(const QSqlDatabase& db)
{
    QSqlQuery q(db);
    if (!q.exec(QStringLiteral("SELECT pg_advisory_xact_lock(%1)").arg(kAdvisoryLockKeyNomenclatureCode)))
        throw std::runtime_error(q.lastError().text().toStdString());
}

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
    const std::optional<Cursor>& cursor,
    const QString& searchText)
{
    const int normalizedLimit = normalizeLimit(limit);
    const int fetchLimit = normalizedLimit + 1;
    const QString trimmedSearch = searchText.trimmed();
    const bool hasSearch = !trimmedSearch.isEmpty();

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    QString sql = baseSelectSql();
    if (hasSearch)
    {
        sql += QStringLiteral(
            "WHERE (n.code ILIKE :search_pattern "
            "   OR n.description ILIKE :search_pattern "
            "   OR COALESCE(n.article, '') ILIKE :search_pattern "
            "   OR COALESCE(u.description, '') ILIKE :search_pattern "
            "   OR CASE WHEN n.service THEN 'yes' ELSE 'no' END ILIKE :search_pattern) ");
    }
    else if (parentId.has_value())
        sql += QStringLiteral(
            "WHERE n.parent_idrref = :parent_id "
            "  AND n.idrref <> :parent_id ");
    else
        sql += QStringLiteral("WHERE n.parent_idrref IS NULL ");

    appendCursorPredicate(sql, cursor);
    sql += QStringLiteral(
        "ORDER BY n.folder DESC, n.code ASC, n.idrref ASC "
        "LIMIT :limit");

    query.prepare(sql);
    if (parentId.has_value())
        query.bindValue(":parent_id", *parentId);
    if (hasSearch)
        query.bindValue(":search_pattern", QStringLiteral("%%1%").arg(trimmedSearch));
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

using SaveStatus = SC::Application::Catalogs::Nomenclature::SaveStatus;
using SaveResult = SC::Application::Catalogs::Nomenclature::NomenclatureSaveResult;
using RecordDto = SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto;
using ItemCommand = SC::Application::Catalogs::Nomenclature::NomenclatureItemUpsertCommand;
using GroupCommand = SC::Application::Catalogs::Nomenclature::NomenclatureGroupUpsertCommand;

QByteArray newId()
{
    return QUuid::createUuid().toRfc4122();
}

SaveResult makeValidationError(const QString& message)
{
    SaveResult result;
    result.status = SaveStatus::ValidationError;
    result.message = message;
    return result;
}

SaveResult makeError(const QString& message)
{
    SaveResult result;
    result.status = SaveStatus::Error;
    result.message = message;
    return result;
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
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor,
    const QString& searchText)
{
    return executePagedQuery(std::nullopt, limit, cursor, searchText);
}

SC::Application::Catalogs::Nomenclature::NomenclatureTreePage
SqlNomenclatureQueryService::fetchChildrenPage(
    const QByteArray& parentId,
    int limit,
    const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor,
    const QString& searchText)
{
    return executePagedQuery(parentId, limit, cursor, searchText);
}

std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto>
SqlNomenclatureQueryService::fetchForEdit(const QByteArray& id)
{
    if (id.isEmpty())
        return std::nullopt;

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT n.idrref, "
        "       n.parent_idrref, "
        "       COALESCE(p.description, '') AS parent_display, "
        "       n.version, "
        "       n.folder, "
        "       n.code, "
        "       n.description, "
        "       COALESCE(n.full_description, ''), "
        "       COALESCE(n.article, ''), "
        "       n.unit_idrref, "
        "       COALESCE(u.description, '') AS unit_display, "
        "       COALESCE(n.service, false) "
        "FROM nomenclature n "
        "LEFT JOIN nomenclature p ON p.idrref = n.parent_idrref "
        "LEFT JOIN units u ON u.idrref = n.unit_idrref "
        "WHERE n.idrref = :id"));
    query.bindValue(":id", id);

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    RecordDto dto;
    dto.id = query.value(0).toByteArray();
    dto.parentId = query.value(1).toByteArray();
    dto.parentDisplay = query.value(2).toString().trimmed();
    dto.version = query.value(3).toInt();
    dto.folder = query.value(4).toBool();
    dto.code = query.value(5).toString();
    dto.name = query.value(6).toString();
    dto.fullDescription = query.value(7).toString();
    dto.article = query.value(8).toString();
    if (!query.value(9).isNull())
        dto.unitId = query.value(9).toByteArray();
    dto.unitDisplay = query.value(10).toString().trimmed();
    dto.service = query.value(11).toBool();

    return dto;
}

QString SqlNomenclatureQueryService::getNextCode()
{
    constexpr int kCodeLength = 11;
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT n.code FROM nomenclature n ORDER BY n.code DESC LIMIT 1"));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    QString maxCode;
    if (query.next())
        maxCode = query.value(0).toString().trimmed();
    return SC::Core::nextCodeFromMax(maxCode, kCodeLength);
}

SC::Application::Catalogs::Nomenclature::NomenclatureSaveResult
SqlNomenclatureQueryService::upsertItem(const ItemCommand& command)
{
    if (command.name.trimmed().isEmpty())
        return makeValidationError(QStringLiteral("Name is required."));
    if (!command.unitId.has_value() || command.unitId->isEmpty())
        return makeValidationError(QStringLiteral("Unit is required for item."));

    QString code = command.code.trimmed();
    const bool needGeneratedCode = code.isEmpty();

    const bool isCreate = !command.id.has_value();
    const QByteArray id = isCreate ? newId() : *command.id;
    const int expectedVersion = command.expectedVersion.value_or(0);
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    try
    {
        SC::Infrastructure::Persistence::UnitOfWork::begin();
        if (needGeneratedCode)
        {
            acquireAdvisoryLockForNomenclatureCode(db);
            code = getNextCode();
        }
        if (isCreate)
        {
            query.prepare(QStringLiteral(
                "INSERT INTO nomenclature "
                "(idrref, version, marked, parent_idrref, folder, code, description, full_description, article, unit_idrref, service) "
                "VALUES (:id, 0, false, :parent_id, false, :code, :name, :full_description, :article, :unit_id, :service)"));
            query.bindValue(":id", id);
            query.bindValue(":parent_id", command.parentId.isEmpty() ? QVariant() : QVariant(command.parentId));
            query.bindValue(":code", code);
            query.bindValue(":name", command.name.trimmed());
            query.bindValue(":full_description", command.fullDescription.trimmed());
            query.bindValue(":article", command.article.trimmed().isEmpty() ? QVariant() : QVariant(command.article.trimmed()));
            query.bindValue(":unit_id", *command.unitId);
            query.bindValue(":service", command.service);

            if (!query.exec())
                throw std::runtime_error(query.lastError().text().toStdString());

            SC::Infrastructure::Persistence::UnitOfWork::commit();
            SaveResult result;
            result.status = SaveStatus::Success;
            result.id = id;
            result.newVersion = 0;
            if (needGeneratedCode)
                result.assignedCode = code;
            return result;
        }

        query.prepare(QStringLiteral(
            "UPDATE nomenclature "
            "SET parent_idrref = :parent_id, "
            "    code = :code, "
            "    description = :name, "
            "    full_description = :full_description, "
            "    article = :article, "
            "    unit_idrref = :unit_id, "
            "    service = :service, "
            "    version = version + 1 "
            "WHERE idrref = :id AND folder = false AND version = :expected_version"));
        query.bindValue(":parent_id", command.parentId.isEmpty() ? QVariant() : QVariant(command.parentId));
        query.bindValue(":code", code);
        query.bindValue(":name", command.name.trimmed());
        query.bindValue(":full_description", command.fullDescription.trimmed());
        query.bindValue(":article", command.article.trimmed().isEmpty() ? QVariant() : QVariant(command.article.trimmed()));
        query.bindValue(":unit_id", *command.unitId);
        query.bindValue(":service", command.service);
        query.bindValue(":id", id);
        query.bindValue(":expected_version", expectedVersion);

        if (!query.exec())
            throw std::runtime_error(query.lastError().text().toStdString());

        if (query.numRowsAffected() == 0)
        {
            SC::Infrastructure::Persistence::UnitOfWork::rollback();
            SaveResult conflict;
            conflict.status = SaveStatus::ConcurrencyConflict;
            conflict.id = id;
            conflict.message = QStringLiteral("Record has been changed by another user.");
            return conflict;
        }

        SC::Infrastructure::Persistence::UnitOfWork::commit();
        SaveResult ok;
        ok.status = SaveStatus::Success;
        ok.id = id;
        ok.newVersion = expectedVersion + 1;
        if (needGeneratedCode)
            ok.assignedCode = code;
        return ok;
    }
    catch (const std::exception& ex)
    {
        SC::Infrastructure::Persistence::UnitOfWork::rollback();
        return makeError(QString::fromUtf8(ex.what()));
    }
}

SC::Application::Catalogs::Nomenclature::NomenclatureSaveResult
SqlNomenclatureQueryService::upsertGroup(const GroupCommand& command)
{
    if (command.name.trimmed().isEmpty())
        return makeValidationError(QStringLiteral("Name is required."));

    QString code = command.code.trimmed();
    const bool needGeneratedCode = code.isEmpty();

    const bool isCreate = !command.id.has_value();
    const QByteArray id = isCreate ? newId() : *command.id;
    const int expectedVersion = command.expectedVersion.value_or(0);
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    try
    {
        SC::Infrastructure::Persistence::UnitOfWork::begin();
        if (needGeneratedCode)
        {
            acquireAdvisoryLockForNomenclatureCode(db);
            code = getNextCode();
        }
        if (isCreate)
        {
            query.prepare(QStringLiteral(
                "INSERT INTO nomenclature "
                "(idrref, version, marked, parent_idrref, folder, code, description, full_description, article, unit_idrref, service) "
                "VALUES (:id, 0, false, :parent_id, true, :code, :name, :full_description, NULL, NULL, NULL)"));
            query.bindValue(":id", id);
            query.bindValue(":parent_id", command.parentId.has_value() ? QVariant(*command.parentId) : QVariant());
            query.bindValue(":code", code);
            query.bindValue(":name", command.name.trimmed());
            query.bindValue(":full_description", command.fullDescription.trimmed());
            if (!query.exec())
                throw std::runtime_error(query.lastError().text().toStdString());

            SC::Infrastructure::Persistence::UnitOfWork::commit();
            SaveResult result;
            result.status = SaveStatus::Success;
            result.id = id;
            result.newVersion = 0;
            if (needGeneratedCode)
                result.assignedCode = code;
            return result;
        }

        query.prepare(QStringLiteral(
            "UPDATE nomenclature "
            "SET parent_idrref = :parent_id, "
            "    code = :code, "
            "    description = :name, "
            "    full_description = :full_description, "
            "    version = version + 1 "
            "WHERE idrref = :id AND folder = true AND version = :expected_version"));
        query.bindValue(":parent_id", command.parentId.has_value() ? QVariant(*command.parentId) : QVariant());
        query.bindValue(":code", code);
        query.bindValue(":name", command.name.trimmed());
        query.bindValue(":full_description", command.fullDescription.trimmed());
        query.bindValue(":id", id);
        query.bindValue(":expected_version", expectedVersion);

        if (!query.exec())
            throw std::runtime_error(query.lastError().text().toStdString());

        if (query.numRowsAffected() == 0)
        {
            SC::Infrastructure::Persistence::UnitOfWork::rollback();
            SaveResult conflict;
            conflict.status = SaveStatus::ConcurrencyConflict;
            conflict.id = id;
            conflict.message = QStringLiteral("Record has been changed by another user.");
            return conflict;
        }

        SC::Infrastructure::Persistence::UnitOfWork::commit();
        SaveResult ok;
        ok.status = SaveStatus::Success;
        ok.id = id;
        ok.newVersion = expectedVersion + 1;
        if (needGeneratedCode)
            ok.assignedCode = code;
        return ok;
    }
    catch (const std::exception& ex)
    {
        SC::Infrastructure::Persistence::UnitOfWork::rollback();
        return makeError(QString::fromUtf8(ex.what()));
    }
}

} // namespace SC::Infrastructure::Catalogs::Nomenclature
