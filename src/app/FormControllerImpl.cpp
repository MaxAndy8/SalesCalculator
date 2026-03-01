#include "FormControllerImpl.h"

#include "application/auth/AuthenticatedUser.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace SC::App
{

FormControllerImpl::FormControllerImpl(const SC::Application::Auth::AuthenticatedUser& user)
    : m_user(user)
{
}

QWidget* FormControllerImpl::getForm(SC::UI::FormType type)
{
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    switch (type)
    {
    case SC::UI::FormType::Catalog_Nomenclature_ListForm:
        widget->setWindowTitle(QWidget::tr("Catalog_Nomenclature_ListForm"));
        layout->addWidget(new QLabel(QWidget::tr("Catalog_Nomenclature_ListForm"), widget));
        break;
    case SC::UI::FormType::Catalog_Users_ListForm:
        widget->setWindowTitle(QWidget::tr("Catalog_Users_ListForm"));
        layout->addWidget(new QLabel(QWidget::tr("Catalog_Users_ListForm"), widget));
        break;
    default:
        widget->setWindowTitle(QWidget::tr("Form"));
        layout->addWidget(new QLabel(QWidget::tr("Unknown form type"), widget));
        break;
    }

    return widget;
}

} // namespace SC::App
