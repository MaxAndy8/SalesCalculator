#pragma once

#include "ui/forms/base/ListForm.h"

#include <QByteArray>

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureQueryService;
class INomenclatureCommandService;
}

namespace SC::Application::Catalogs::Units
{
class IUnitCommandService;
class IUnitQueryService;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureTreeModel;

class NomenclatureListForm : public SC::UI::Forms::Base::ListForm
{
    Q_OBJECT

public:
    explicit NomenclatureListForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
        SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService = nullptr,
        SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService = nullptr,
        SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService = nullptr,
        QWidget* parent = nullptr);
    ~NomenclatureListForm() override = default;

protected:
    void handleCreateItemRequested() override;
    void handleCreateFolderRequested() override;
    void handleEditRequested() override;
    void handleDeleteRequested() override;
    void handleRefreshRequested() override;
    void handleCopyRequested() override;

private:
    void refreshPreserveSelection(const QByteArray& preferredId = QByteArray());
    QModelIndex findIndexByIdWithFetch(const QByteArray& id);

private:
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* m_queryService = nullptr;
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* m_commandService = nullptr;
    SC::Application::Catalogs::Units::IUnitQueryService* m_unitQueryService = nullptr;
    SC::Application::Catalogs::Units::IUnitCommandService* m_unitCommandService = nullptr;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
