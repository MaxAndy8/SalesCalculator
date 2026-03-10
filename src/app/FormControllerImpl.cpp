#include "FormControllerImpl.h"

#include "application/auth/AuthenticatedUser.h"
#include "application/catalogs/nomenclature/INomenclatureCommandService.h"
#include "application/catalogs/nomenclature/INomenclatureQueryService.h"
#include "application/catalogs/units/IUnitCommandService.h"
#include "application/catalogs/units/IUnitQueryService.h"
#include "application/forms/ReferenceFieldPolicy.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureChoiceForm.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureGroupForm.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureItemForm.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureListForm.h"
#include "ui/forms/catalogs/units/UnitChoiceForm.h"
#include "ui/forms/catalogs/units/UnitItemForm.h"

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
            auto* nomenclatureQueryService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureQueryService*>(it->second.get());
            auto* nomenclatureCommandService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureCommandService*>(it->second.get());

            SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService = nullptr;
            SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService = nullptr;
            auto unitIt = m_queryServices.find(SC::UI::FormType::Catalog_Units_ChoiceForm);
            if (unitIt != m_queryServices.end() && unitIt->second != nullptr)
            {
                unitQueryService = dynamic_cast<SC::Application::Catalogs::Units::IUnitQueryService*>(unitIt->second.get());
                unitCommandService = dynamic_cast<SC::Application::Catalogs::Units::IUnitCommandService*>(unitIt->second.get());
            }

            if (nomenclatureQueryService != nullptr)
            {
                return new SC::UI::Forms::Catalogs::Nomenclature::NomenclatureListForm(
                    nomenclatureQueryService,
                    nomenclatureCommandService,
                    unitQueryService,
                    unitCommandService);
            }
        }

        auto* widget = new QWidget;
        auto* layout = new QVBoxLayout(widget);
        widget->setWindowTitle(QWidget::tr("Catalog_Nomenclature_ListForm"));
        layout->addWidget(new QLabel(QWidget::tr("Nomenclature tree query service is not registered"), widget));
        return widget;
    }
    case SC::UI::FormType::Catalog_Nomenclature_ItemForm:
    {
        auto nomIt = m_queryServices.find(SC::UI::FormType::Catalog_Nomenclature_ListForm);
        auto unitIt = m_queryServices.find(SC::UI::FormType::Catalog_Units_ChoiceForm);
        if (nomIt != m_queryServices.end() && nomIt->second != nullptr)
        {
            auto* queryService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureQueryService*>(nomIt->second.get());
            auto* commandService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureCommandService*>(nomIt->second.get());
            SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService = nullptr;
            SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService = nullptr;
            if (unitIt != m_queryServices.end() && unitIt->second != nullptr)
            {
                unitQueryService = dynamic_cast<SC::Application::Catalogs::Units::IUnitQueryService*>(unitIt->second.get());
                unitCommandService = dynamic_cast<SC::Application::Catalogs::Units::IUnitCommandService*>(unitIt->second.get());
            }
            if (queryService != nullptr && commandService != nullptr)
                return new SC::UI::Forms::Catalogs::Nomenclature::NomenclatureItemForm(queryService, commandService, unitQueryService, unitCommandService);
        }
        break;
    }
    case SC::UI::FormType::Catalog_Nomenclature_GroupForm:
    {
        auto nomIt = m_queryServices.find(SC::UI::FormType::Catalog_Nomenclature_ListForm);
        if (nomIt != m_queryServices.end() && nomIt->second != nullptr)
        {
            auto* queryService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureQueryService*>(nomIt->second.get());
            auto* commandService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureCommandService*>(nomIt->second.get());
            if (queryService != nullptr && commandService != nullptr)
                return new SC::UI::Forms::Catalogs::Nomenclature::NomenclatureGroupForm(queryService, commandService);
        }
        break;
    }
    case SC::UI::FormType::Catalog_Nomenclature_GroupChoiceForm:
    {
        auto nomIt = m_queryServices.find(SC::UI::FormType::Catalog_Nomenclature_ListForm);
        if (nomIt != m_queryServices.end() && nomIt->second != nullptr)
        {
            auto* queryService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureQueryService*>(nomIt->second.get());
            auto* commandService =
                dynamic_cast<SC::Application::Catalogs::Nomenclature::INomenclatureCommandService*>(nomIt->second.get());
            SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService = nullptr;
            auto unitIt = m_queryServices.find(SC::UI::FormType::Catalog_Units_ChoiceForm);
            if (unitIt != m_queryServices.end() && unitIt->second != nullptr)
                unitQueryService = dynamic_cast<SC::Application::Catalogs::Units::IUnitQueryService*>(unitIt->second.get());
            if (queryService != nullptr)
            {
                return new SC::UI::Forms::Catalogs::Nomenclature::NomenclatureChoiceForm(
                    queryService,
                    SC::Application::Forms::AllowedNodeKinds::FoldersOnly,
                    commandService,
                    unitQueryService);
            }
        }
        break;
    }
    case SC::UI::FormType::Catalog_Units_ChoiceForm:
    {
        auto unitIt = m_queryServices.find(SC::UI::FormType::Catalog_Units_ChoiceForm);
        if (unitIt != m_queryServices.end() && unitIt->second != nullptr)
        {
            auto* unitService =
                dynamic_cast<SC::Application::Catalogs::Units::IUnitQueryService*>(unitIt->second.get());
            if (unitService != nullptr)
                return new SC::UI::Forms::Catalogs::Units::UnitChoiceForm(unitService);
        }
        break;
    }
    case SC::UI::FormType::Catalog_Units_ItemForm:
    {
        auto unitIt = m_queryServices.find(SC::UI::FormType::Catalog_Units_ChoiceForm);
        if (unitIt != m_queryServices.end() && unitIt->second != nullptr)
        {
            auto* unitQueryService =
                dynamic_cast<SC::Application::Catalogs::Units::IUnitQueryService*>(unitIt->second.get());
            auto* unitCommandService =
                dynamic_cast<SC::Application::Catalogs::Units::IUnitCommandService*>(unitIt->second.get());
            if (unitQueryService != nullptr && unitCommandService != nullptr)
                return new SC::UI::Forms::Catalogs::Units::UnitItemForm(unitQueryService, unitCommandService);
        }
        break;
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

    return nullptr;
}

} // namespace SC::App
