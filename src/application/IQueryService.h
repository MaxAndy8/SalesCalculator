#pragma once

#include "ui/formController/FormType.h"

namespace SC::Application
{

class IQueryService
{
public:
    virtual ~IQueryService() = default;

    virtual SC::UI::FormType formType() const = 0;
};

} // namespace SC::Application
