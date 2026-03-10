#pragma once

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

} // namespace SC::Application::Forms
