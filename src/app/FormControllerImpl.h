#pragma once

#include "ui/formController/IFormController.h"
#include "application/auth/AuthenticatedUser.h"
#include "application/IQueryService.h"

#include <map>
#include <memory>

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureQueryService;
}

namespace SC::App
{
// Impl — це скорочення від Implementation (реалізація). Він виконує контракт, описаний в інтерфейсі IFormController.
class FormControllerImpl final : public SC::UI::IFormController
{
public:
    explicit FormControllerImpl(const SC::Application::Auth::AuthenticatedUser& user);

    void addSqlQueryService(
        SC::Application::IQueryService* queryService);

    QWidget* getForm(SC::UI::FormType type) override;

private:
    SC::Application::Auth::AuthenticatedUser m_user;
    std::map<SC::UI::FormType, std::unique_ptr<SC::Application::IQueryService>> m_queryServices;
};

} // namespace SC::App
