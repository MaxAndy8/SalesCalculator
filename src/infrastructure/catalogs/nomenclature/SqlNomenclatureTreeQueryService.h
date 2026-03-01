#pragma once

#include "application/catalogs/nomenclature/INomenclatureTreeQueryService.h"

namespace SC::Infrastructure::Catalogs::Nomenclature
{

class SqlNomenclatureTreeQueryService final
    : public SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService
{
public:
    SC::UI::FormType formType() const override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchRootPage(
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor) override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchChildrenPage(
        const QByteArray& parentId,
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor) override;
};

} // namespace SC::Infrastructure::Catalogs::Nomenclature
