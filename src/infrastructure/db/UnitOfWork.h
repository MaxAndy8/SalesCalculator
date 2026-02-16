#pragma once
namespace SC::Infrastructure::Persistence {

class UnitOfWork {
public:
    static void begin();
    static void commit();
    static void rollback();
    static bool isActive();
private:
    static int& depth();
    static bool& rollbackOnly();
};

} // namespace SC::Infrastructure::Persistence
