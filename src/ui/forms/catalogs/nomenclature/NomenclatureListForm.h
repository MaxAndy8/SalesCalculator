#pragma once

#include "ui/forms/base/ListForm.h"

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureTreeQueryService;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureTreeModel;

class NomenclatureListForm final : public SC::UI::Forms::Base::ListForm
{
    Q_OBJECT

public:
    explicit NomenclatureListForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* queryService,
        QWidget* parent = nullptr);
    ~NomenclatureListForm() override = default;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
