#pragma once

#include "application/forms/ReferenceFieldPolicy.h"
#include "ui/forms/catalogs/nomenclature/NomenclatureListForm.h"

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureChoiceForm final : public NomenclatureListForm
{
    Q_OBJECT

public:
    explicit NomenclatureChoiceForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
        SC::Application::Forms::AllowedNodeKinds allowedKinds,
        SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService = nullptr,
        SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService = nullptr,
        QWidget* parent = nullptr);

signals:
    void selectionAccepted(
        const QByteArray& id,
        const QString& displayText,
        SC::Application::Forms::AllowedNodeKinds nodeKind);

private:
    void tryAcceptCurrentSelection();
    void hideDisallowedRows();
    void applyRowVisibility(const QModelIndex& parentIndex);

private:
    SC::Application::Forms::AllowedNodeKinds m_allowedKinds;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
