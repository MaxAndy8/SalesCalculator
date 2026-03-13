#pragma once

#include "application/IQueryService.h"
#include "application/forms/ReferenceFieldPolicy.h"

#include <QByteArray>
#include <QString>
#include <QVector>

#include <vector>

namespace SC::Application::Catalogs::Units
{

struct UnitDto
{
    QByteArray id;
    QString code;
    QString name;
};

class IUnitQueryService : public SC::Application::IQueryService
{
public:
    virtual ~IUnitQueryService() = default;
    virtual std::vector<UnitDto> fetchAll(const QString& searchText = QString()) = 0;

    /// Пошук для автозаповнення: ILIKE по code та description, формат рядка description + " " + code.
    virtual QVector<SC::Application::Forms::AutocompleteEntry> searchForAutocomplete(
        const QString& searchText,
        int limit) = 0;
};

} // namespace SC::Application::Catalogs::Units
