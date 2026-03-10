#pragma once

#include <QString>

class QWidget;

namespace SC::UI::Forms::Base::MdiUtils
{

QWidget* findOpenedFormById(QWidget* context, const QString& idMdiSubWindow);
bool openFormInMdi(
    QWidget* context,
    QWidget* form,
    const QString& idMdiSubWindow,
    const QString& windowTitle);
bool closeContainingMdiSubWindow(QWidget* widget);

} // namespace SC::UI::Forms::Base::MdiUtils
