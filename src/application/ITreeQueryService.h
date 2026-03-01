#pragma once

#include "ui/formController/FormType.h"

namespace SC::Application
{

class ITreeQueryService
{
public:
    virtual ~ITreeQueryService() = default;

    virtual SC::UI::FormType formType() const = 0;
};

} // namespace SC::Application
