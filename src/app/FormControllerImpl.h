#pragma once

#include "ui/formController/IFormController.h"
#include "application/auth/AuthenticatedUser.h"

namespace SC::App
{
// Impl — це скорочення від Implementation (реалізація). Він виконує контракт, описаний в інтерфейсі IFormController.
class FormControllerImpl final : public SC::UI::IFormController
{
public:
    explicit FormControllerImpl(const SC::Application::Auth::AuthenticatedUser& user);

    QWidget* getForm(SC::UI::FormType type) override;

private:
    SC::Application::Auth::AuthenticatedUser m_user;
};

} // namespace SC::App
