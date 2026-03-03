#include "FormControllerImpl.h"

#include "application/auth/AuthenticatedUser.h"
#include "application/catalogs/nomenclature/INomenclatureQueryService.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureListForm.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace SC::App
{

FormControllerImpl::FormControllerImpl(const SC::Application::Auth::AuthenticatedUser& user)
    : m_user(user)
{
}

void FormControllerImpl::addSqlQueryService(
    SC::Application::IQueryService* queryService)
{
    if (queryService == nullptr)
        return;

    const auto formType = queryService->formType();
    m_queryServices[formType] = std::unique_ptr<SC::Application::IQueryService>(queryService);
}

QWidget* FormControllerImpl::getForm(SC::UI::FormType type)
{
    switch (type)
    {
    case SC::UI::FormType::Catalog_Nomenclature_ListForm:
    {
        auto it = m_queryServices.find(type);
        if (it != m_queryServices.end() && it->second != nullptr)
        {
            auto* nomenclatureService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureQueryService*>(it->second.get());
            if (nomenclatureService != nullptr)
                return new SC::UI::Forms::Catalogs::Nomenclature::NomenclatureListForm(nomenclatureService);
        }

        auto* widget = new QWidget;
        auto* layout = new QVBoxLayout(widget);
        widget->setWindowTitle(QWidget::tr("Catalog_Nomenclature_ListForm"));
        layout->addWidget(new QLabel(QWidget::tr("Nomenclature tree query service is not registered"), widget));
        return widget;
    }
    case SC::UI::FormType::Catalog_Users_ListForm:
    {
        auto* widget = new QWidget;
        auto* layout = new QVBoxLayout(widget);
        widget->setWindowTitle(QWidget::tr("Catalog_Users_ListForm"));
        layout->addWidget(new QLabel(QWidget::tr("Catalog_Users_ListForm"), widget));
        return widget;
    }
    default:
    {
        auto* widget = new QWidget;
        auto* layout = new QVBoxLayout(widget);
        widget->setWindowTitle(QWidget::tr("Form"));
        layout->addWidget(new QLabel(QWidget::tr("Unknown form type"), widget));
        return widget;
    }
    }
}

} // namespace SC::App
