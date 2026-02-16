#pragma once

#include "DbConnectionProvider.h"
#include "application/database/DatabaseConnectionInfo.h"

namespace SC::Infrastructure::DB
{

class DbProviderGuard final
{
public:
    explicit DbProviderGuard(const SC::Application::Database::DatabaseConnectionInfo& info);
    ~DbProviderGuard();

    DbProviderGuard(const DbProviderGuard&) = delete;
    DbProviderGuard& operator=(const DbProviderGuard&) = delete;

private:
    bool m_active = false;
};

} // namespace SC::Infrastructure::DB
