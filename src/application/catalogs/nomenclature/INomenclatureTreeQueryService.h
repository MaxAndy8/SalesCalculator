#pragma once

#include "application/ITreeQueryService.h"

#include <QByteArray>
#include <QString>

#include <optional>
#include <vector>

namespace SC::Application::Catalogs::Nomenclature
{

struct NomenclatureTreeCursor
{
    bool lastIsGroup = false;
    QString lastCode;
    QByteArray lastId;
};

struct NomenclatureTreeNodeDto
{
    QByteArray id;
    std::optional<QByteArray> parentId;
    bool isGroup = false;
    QString code;
    QString name;
    bool hasChildren = false;
};

struct NomenclatureTreePage
{
    std::vector<NomenclatureTreeNodeDto> items;
    std::optional<NomenclatureTreeCursor> nextCursor;
};

class INomenclatureTreeQueryService : public SC::Application::ITreeQueryService
{
public:
    virtual ~INomenclatureTreeQueryService() = default;

    virtual NomenclatureTreePage fetchRootPage(
        int limit,
        const std::optional<NomenclatureTreeCursor>& cursor) = 0;

    virtual NomenclatureTreePage fetchChildrenPage(
        const QByteArray& parentId,
        int limit,
        const std::optional<NomenclatureTreeCursor>& cursor) = 0;
};

} // namespace SC::Application::Catalogs::Nomenclature
