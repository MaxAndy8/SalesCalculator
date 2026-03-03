#pragma once

#include "application/IQueryService.h"

#include <QByteArray>

namespace SC::Application::Catalogs
{

class ICatalogQueryService : public SC::Application::IQueryService
{
public:
    virtual ~ICatalogQueryService() = default;

    virtual bool CreateItem() = 0;
    virtual bool CreateFolder() = 0;
    virtual bool Write(const QByteArray& id) = 0;
    virtual bool Delete(const QByteArray& id) = 0;
};

} // namespace SC::Application::Catalogs
