#pragma once

#include "application/catalogs/nomenclature/INomenclatureQueryService.h"

namespace SC::Infrastructure::Catalogs::Nomenclature
{

class SqlNomenclatureQueryService final
    : public SC::Application::Catalogs::Nomenclature::INomenclatureQueryService
{
public:
    SC::UI::FormType formType() const override;
    bool CreateItem() override;
    bool CreateFolder() override;
    bool Write(const QByteArray& id) override;
    bool Delete(const QByteArray& id) override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchRootPage(
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor) override;

    SC::Application::Catalogs::Nomenclature::NomenclatureTreePage fetchChildrenPage(
        const QByteArray& parentId,
        int limit,
        const std::optional<SC::Application::Catalogs::Nomenclature::NomenclatureTreeCursor>& cursor) override;
};

} // namespace SC::Infrastructure::Catalogs::Nomenclature
