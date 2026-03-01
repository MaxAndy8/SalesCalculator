#pragma once

#include "FormType.h"
#include <QWidget>

namespace SC::UI
{

class IFormController
{
public:
    virtual ~IFormController() = default;

    virtual QWidget* getForm(FormType type) = 0;

};

} // namespace SC::UI
