#include "UnitOfWork.h"
#include "DbConnectionProvider.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>

namespace SC::Infrastructure::Persistence {

int& UnitOfWork::depth()
{
    thread_local int depth_ = 0;
    return depth_;
}
bool& UnitOfWork::rollbackOnly()
{
    thread_local bool rollbackOnly_ = false;
    return rollbackOnly_;
}
void UnitOfWork::begin()
{
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    if (depth() == 0) {
        if (!db.transaction()) {
            throw std::runtime_error(
                ("BEGIN failed: " + db.lastError().text()).toStdString()
                );
        }
        rollbackOnly() = false;
    }
    ++depth();
}
void UnitOfWork::commit()
{
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    if (depth() == 0) {
        throw std::logic_error("UnitOfWork::commit called without active transaction");
    }
    --depth();
    if (depth() == 0) {
        if (rollbackOnly()) {
            if (!db.rollback()) {
                throw std::runtime_error(
                    ("ROLLBACK failed: " + db.lastError().text()).toStdString()
                    );
            }
        } else {
            if (!db.commit()) {
                throw std::runtime_error(
                    ("COMMIT failed: " + db.lastError().text()).toStdString()
                    );
            }
        }
    }
}
void UnitOfWork::rollback()
{
    auto db = SC::Infrastructure::DB::DbConnectionProvider::current();
    if (depth() == 0) {
        return; // no-op
    }
    rollbackOnly() = true;
    depth() = 0;
    if (!db.rollback()) {
        throw std::runtime_error(
            ("ROLLBACK failed: " + db.lastError().text()).toStdString()
            );
    }
}
bool UnitOfWork::isActive()
{
    return depth() > 0;
}

} // namespace SC::Infrastructure::Persistence
