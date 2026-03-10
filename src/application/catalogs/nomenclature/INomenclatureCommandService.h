#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace SC::Application::Catalogs::Nomenclature
{

enum class SaveStatus
{
    Success = 0,
    ValidationError = 1,
    ConcurrencyConflict = 2,
    Error = 3
};

struct NomenclatureRecordDto
{
    QByteArray id;
    QByteArray parentId;
    QString parentDisplay;
    int version = 0;
    bool folder = false;
    QString code;
    QString name;
    QString fullDescription;
    QString article;
    std::optional<QByteArray> unitId;
    QString unitDisplay;
    bool service = false;
};

struct NomenclatureItemUpsertCommand
{
    std::optional<QByteArray> id;
    std::optional<int> expectedVersion;
    QByteArray parentId;
    QString code;
    QString name;
    QString fullDescription;
    QString article;
    std::optional<QByteArray> unitId;
    bool service = false;
};

struct NomenclatureGroupUpsertCommand
{
    std::optional<QByteArray> id;
    std::optional<int> expectedVersion;
    std::optional<QByteArray> parentId;
    QString code;
    QString name;
    QString fullDescription;
};

struct NomenclatureSaveResult
{
    SaveStatus status = SaveStatus::Error;
    QByteArray id;
    int newVersion = 0;
    QString message;
    /** Якщо запис збережено з автонумерацією коду — тут присвоєний код для відображення у формі. */
    std::optional<QString> assignedCode;
};

class INomenclatureCommandService
{
public:
    virtual ~INomenclatureCommandService() = default;

    virtual std::optional<NomenclatureRecordDto> fetchForEdit(const QByteArray& id) = 0;
    virtual NomenclatureSaveResult upsertItem(const NomenclatureItemUpsertCommand& command) = 0;
    virtual NomenclatureSaveResult upsertGroup(const NomenclatureGroupUpsertCommand& command) = 0;

    /** Повертає наступний код по всій таблиці (алфавітне сортування, доповнення нулями). */
    virtual QString getNextCode() = 0;
};

} // namespace SC::Application::Catalogs::Nomenclature
