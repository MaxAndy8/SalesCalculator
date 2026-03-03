#pragma once

#include "ui/forms/base/ListForm.h"

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureQueryService;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureTreeModel;

class NomenclatureListForm final : public SC::UI::Forms::Base::ListForm
{
    Q_OBJECT

public:
    explicit NomenclatureListForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
        QWidget* parent = nullptr);
    ~NomenclatureListForm() override = default;

private:
    void onDeleteButtonClicked();

private:
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* m_queryService = nullptr;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
