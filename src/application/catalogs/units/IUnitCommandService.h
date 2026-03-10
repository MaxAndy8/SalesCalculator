#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace SC::Application::Catalogs::Units
{

enum class SaveStatus
{
    Success = 0,
    ValidationError = 1,
    ConcurrencyConflict = 2,
    Error = 3
};

struct UnitRecordDto
{
    QByteArray id;
    int version = 0;
    QString code;
    QString description;
    QString fullDescription;
};

struct UnitUpsertCommand
{
    std::optional<QByteArray> id;
    std::optional<int> expectedVersion;
    QString code;
    QString description;
    QString fullDescription;
};

struct UnitSaveResult
{
    SaveStatus status = SaveStatus::Error;
    QByteArray id;
    int newVersion = 0;
    QString message;
    std::optional<QString> assignedCode;
};

class IUnitCommandService
{
public:
    virtual ~IUnitCommandService() = default;

    virtual std::optional<UnitRecordDto> fetchForEdit(const QByteArray& id) = 0;
    virtual UnitSaveResult upsert(const UnitUpsertCommand& command) = 0;
};

} // namespace SC::Application::Catalogs::Units
