#pragma once

#include "application/catalogs/units/IUnitCommandService.h"
#include "application/catalogs/units/IUnitQueryService.h"

namespace SC::Infrastructure::Catalogs::Units
{

class SqlUnitQueryService final
    : public SC::Application::Catalogs::Units::IUnitQueryService,
      public SC::Application::Catalogs::Units::IUnitCommandService
{
public:
    SC::UI::FormType formType() const override;
    std::vector<SC::Application::Catalogs::Units::UnitDto> fetchAll(
        const QString& searchText = QString()) override;

    std::optional<SC::Application::Catalogs::Units::UnitRecordDto> fetchForEdit(
        const QByteArray& id) override;
    SC::Application::Catalogs::Units::UnitSaveResult upsert(
        const SC::Application::Catalogs::Units::UnitUpsertCommand& command) override;
};

} // namespace SC::Infrastructure::Catalogs::Units
