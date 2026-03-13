#pragma once

#include <QByteArray>
#include <QString>

namespace SC::Application::Forms
{

// Defines which node kinds a reference field can accept.
// This is form metadata and is intentionally not tied to one entity.
enum class AllowedNodeKinds
{
    ItemsOnly = 0,
    FoldersOnly = 1,
    ItemsAndFolders = 2
};

// Один варіант для автозаповнення поля посилання (довідник/документ).
struct AutocompleteEntry
{
    QByteArray id;
    QString displayText;
    AllowedNodeKinds nodeKind = AllowedNodeKinds::ItemsOnly;
};

} // namespace SC::Application::Forms
