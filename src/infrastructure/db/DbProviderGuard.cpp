#include "DbProviderGuard.h"

namespace SC::Infrastructure::DB
{
// Додано DbProviderGuard::
DbProviderGuard::DbProviderGuard(const SC::Application::Database::DatabaseConnectionInfo& info)
{
    DbConnectionProvider::initialize(info);
    m_active = true;
}

// Додано DbProviderGuard::
DbProviderGuard::~DbProviderGuard()
{
    if (m_active)
        DbConnectionProvider::shutdown();
}

} // namespace SC::Infrastructure::DB





