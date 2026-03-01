#pragma once

#include "ui/formController/IFormController.h"
#include "application/auth/AuthenticatedUser.h"
#include "application/ITreeQueryService.h"

#include <map>

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureTreeQueryService;
}

namespace SC::App
{
// Impl — це скорочення від Implementation (реалізація). Він виконує контракт, описаний в інтерфейсі IFormController.
class FormControllerImpl final : public SC::UI::IFormController
{
public:
    explicit FormControllerImpl(const SC::Application::Auth::AuthenticatedUser& user);
    ~FormControllerImpl() override;

    void addSqlTreeQueryService(
        SC::Application::ITreeQueryService* queryService);

    QWidget* getForm(SC::UI::FormType type) override;

private:
    SC::Application::Auth::AuthenticatedUser m_user;
    std::map<SC::UI::FormType, SC::Application::ITreeQueryService*> m_queryServices;
};

} // namespace SC::App
