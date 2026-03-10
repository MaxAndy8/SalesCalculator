#pragma once

#include "application/catalogs/nomenclature/INomenclatureQueryService.h"
#include "application/catalogs/nomenclature/INomenclatureCommandService.h"

namespace SC::Infrastructure::Catalogs::Nomenclature
{

class SqlNomenclatureQueryService final
    : public SC::Application::Catalogs::Nomenclature::INomenclatureQueryService,
      public SC::Application::Catalogs::Nomenclature::INomenclatureCommandService
{
public:
    SC::UI::FormType formType() const override;
    bool CreateItem() override;
    bool CreateFolder() override;
    bool Write(const QByteArray& id) override;
    bool Delete(const QByteArray& id) override;
    SC::Application::Catalogs::Nomenclature::ToggleDeletionMarkResult ToggleDeletionMarkForSelection(
        const std::vector<QByteArray>& selectedIds) override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchRootPage(
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor,
        const QString& searchText) override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchChildrenPage(
        const QByteArray& parentId,
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor,
        const QString& searchText) override;

    std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto> fetchForEdit(
        const QByteArray& id) override;
    QString getNextCode() override;
    SC::Application::Catalogs::Nomenclature::NomenclatureSaveResult upsertItem(
        const SC::Application::Catalogs::Nomenclature::NomenclatureItemUpsertCommand& command) override;
    SC::Application::Catalogs::Nomenclature::NomenclatureSaveResult upsertGroup(
        const SC::Application::Catalogs::Nomenclature::NomenclatureGroupUpsertCommand& command) override;
};

} // namespace SC::Infrastructure::Catalogs::Nomenclature
