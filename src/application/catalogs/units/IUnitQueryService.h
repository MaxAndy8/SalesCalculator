#pragma once

#include "application/IQueryService.h"

#include <QByteArray>
#include <QString>

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
};

} // namespace SC::Application::Catalogs::Units
