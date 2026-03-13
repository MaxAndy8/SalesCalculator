#include "SqlUnitQueryService.h"

#include "application/forms/ReferenceFieldPolicy.h"
#include "infrastructure/db/DbConnectionProvider.h"
#include "infrastructure/db/UnitOfWork.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QUuid>

#include <stdexcept>

namespace SC::Infrastructure::Catalogs::Units
{

namespace
{
using UnitSaveResult = SC::Application::Catalogs::Units::UnitSaveResult;
using SaveStatus = SC::Application::Catalogs::Units::SaveStatus;

QByteArray newId()
{
    return QUuid::createUuid().toRfc4122();
}
} // namespace

SC::UI::FormType SqlUnitQueryService::formType() const
{
    return SC::UI::FormType::Catalog_Units_ChoiceForm;
}

std::vector<SC::Application::Catalogs::Units::UnitDto> SqlUnitQueryService::fetchAll(
    const QString& searchText)
{
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    const QString trimmedSearch = searchText.trimmed();
    if (trimmedSearch.isEmpty())
    {
        query.prepare(QStringLiteral(
            "SELECT idrref, code, description "
            "FROM units "
            "ORDER BY code ASC"));
    }
    else
    {
        query.prepare(QStringLiteral(
            "SELECT idrref, code, description "
            "FROM units "
            "WHERE code ILIKE :search OR description ILIKE :search "
            "ORDER BY code ASC"));
        query.bindValue(QStringLiteral(":search"), QStringLiteral("%%1%").arg(trimmedSearch));
    }

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<SC::Application::Catalogs::Units::UnitDto> result;
    while (query.next())
    {
        SC::Application::Catalogs::Units::UnitDto dto;
        dto.id = query.value(0).toByteArray();
        dto.code = query.value(1).toString();
        dto.name = query.value(2).toString();
        result.push_back(std::move(dto));
    }

    return result;
}

QVector<SC::Application::Forms::AutocompleteEntry> SqlUnitQueryService::searchForAutocomplete(
    const QString& searchText,
    int limit)
{
    using SC::Application::Forms::AutocompleteEntry;
    using SC::Application::Forms::AllowedNodeKinds;

    const QString trimmed = searchText.trimmed();
    if (trimmed.isEmpty())
        return {};

    const int safeLimit = std::max(1, std::min(limit, 50));
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);

    query.prepare(QStringLiteral(
        "SELECT idrref, code, description FROM units "
        "WHERE code ILIKE :search OR description ILIKE :search "
        "ORDER BY code ASC LIMIT :limit"));
    query.bindValue(QStringLiteral(":search"), QStringLiteral("%%1%").arg(trimmed));
    query.bindValue(QStringLiteral(":limit"), safeLimit);

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    QVector<AutocompleteEntry> result;
    while (query.next())
    {
        AutocompleteEntry entry;
        entry.id = query.value(0).toByteArray();
        entry.nodeKind = AllowedNodeKinds::ItemsOnly;
        const QString code = query.value(1).toString().trimmed();
        const QString description = query.value(2).toString().trimmed();
        entry.displayText = description + QStringLiteral(" ") + code;
        result.append(entry);
    }
    return result;
}

std::optional<SC::Application::Catalogs::Units::UnitRecordDto> SqlUnitQueryService::fetchForEdit(
    const QByteArray& id)
{
    if (id.isEmpty())
        return std::nullopt;

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT idrref, version, code, description, full_description "
        "FROM units "
        "WHERE idrref = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec() || !query.next())
        return std::nullopt;

    SC::Application::Catalogs::Units::UnitRecordDto dto;
    dto.id = query.value(0).toByteArray();
    dto.version = query.value(1).toInt();
    dto.code = query.value(2).toString();
    dto.description = query.value(3).toString();
    dto.fullDescription = query.value(4).toString();
    return dto;
}

UnitSaveResult SqlUnitQueryService::upsert(
    const SC::Application::Catalogs::Units::UnitUpsertCommand& command)
{
    const bool isCreate = !command.id.has_value();
    const QByteArray id = isCreate ? newId() : *command.id;
    const int expectedVersion = command.expectedVersion.value_or(0);

    const QString code = command.code.trimmed();
    const QString description = command.description.trimmed();
    const QString fullDescription = command.fullDescription.trimmed();

    if (code.isEmpty())
    {
        UnitSaveResult r;
        r.status = SaveStatus::ValidationError;
        r.message = QStringLiteral("Code is required.");
        return r;
    }
    if (description.isEmpty())
    {
        UnitSaveResult r;
        r.status = SaveStatus::ValidationError;
        r.message = QStringLiteral("Description is required.");
        return r;
    }

    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();

    try
    {
        SC::Infrastructure::Persistence::UnitOfWork::begin();
        QSqlQuery query(db);

        if (isCreate)
        {
            query.prepare(QStringLiteral(
                "INSERT INTO units (idrref, version, marked, code, description, full_description) "
                "VALUES (:id, 0, false, :code, :description, :full_description)"));
            query.bindValue(QStringLiteral(":id"), id);
            query.bindValue(QStringLiteral(":code"), code);
            query.bindValue(QStringLiteral(":description"), description);
            query.bindValue(QStringLiteral(":full_description"), fullDescription);

            if (!query.exec())
            {
                const QString err = query.lastError().text();
                SC::Infrastructure::Persistence::UnitOfWork::rollback();
                if (err.contains(QStringLiteral("unique"), Qt::CaseInsensitive) ||
                    err.contains(QStringLiteral("duplicate"), Qt::CaseInsensitive))
                {
                    UnitSaveResult r;
                    r.status = SaveStatus::ValidationError;
                    r.message = QStringLiteral("Unit with this code or description already exists.");
                    return r;
                }
                throw std::runtime_error(err.toStdString());
            }

            SC::Infrastructure::Persistence::UnitOfWork::commit();
            UnitSaveResult result;
            result.status = SaveStatus::Success;
            result.id = id;
            result.newVersion = 0;
            return result;
        }

        query.prepare(QStringLiteral(
            "UPDATE units "
            "SET version = version + 1, code = :code, description = :description, full_description = :full_description "
            "WHERE idrref = :id AND version = :expected_version"));
        query.bindValue(QStringLiteral(":code"), code);
        query.bindValue(QStringLiteral(":description"), description);
        query.bindValue(QStringLiteral(":full_description"), fullDescription);
        query.bindValue(QStringLiteral(":id"), id);
        query.bindValue(QStringLiteral(":expected_version"), expectedVersion);

        if (!query.exec())
        {
            const QString err = query.lastError().text();
            SC::Infrastructure::Persistence::UnitOfWork::rollback();
            if (err.contains(QStringLiteral("unique"), Qt::CaseInsensitive) ||
                err.contains(QStringLiteral("duplicate"), Qt::CaseInsensitive))
            {
                UnitSaveResult r;
                r.status = SaveStatus::ValidationError;
                r.message = QStringLiteral("Unit with this code or description already exists.");
                return r;
            }
            throw std::runtime_error(err.toStdString());
        }

        SC::Infrastructure::Persistence::UnitOfWork::commit();

        if (query.numRowsAffected() == 0)
        {
            UnitSaveResult conflict;
            conflict.status = SaveStatus::ConcurrencyConflict;
            conflict.id = id;
            conflict.message = QStringLiteral("Record has been changed by another user.");
            return conflict;
        }

        UnitSaveResult ok;
        ok.status = SaveStatus::Success;
        ok.id = id;
        ok.newVersion = expectedVersion + 1;
        return ok;
    }
    catch (const std::exception& e)
    {
        SC::Infrastructure::Persistence::UnitOfWork::rollback();
        UnitSaveResult r;
        r.status = SaveStatus::Error;
        r.message = QString::fromStdString(e.what());
        return r;
    }
}

} // namespace SC::Infrastructure::Catalogs::Units
