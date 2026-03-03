#pragma once

#include "application/IQueryService.h"

#include <QByteArray>

namespace SC::Application::Documents
{

class IDocumentQueryService : public SC::Application::IQueryService
{
public:
    virtual ~IDocumentQueryService() = default;

    virtual bool CreateDocument() = 0;
    virtual bool Write(const QByteArray& id) = 0;
    virtual bool Delete(const QByteArray& id) = 0;
};

} // namespace SC::Application::Documents
