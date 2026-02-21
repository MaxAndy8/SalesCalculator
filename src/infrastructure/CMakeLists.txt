
set(SC_INFRASTRUCTURE_TARGET sc_infrastructure)

set(SC_INFRASTRUCTURE_SOURCES
    db/DbConnectionProvider.h
    db/DbConnectionProvider.cpp
    db/UnitOfWork.h
    db/UnitOfWork.cpp
    db/DbProviderGuard.h
    db/DbProviderGuard.cpp
    auth/SqlAuthService.h
    auth/SqlAuthService.cpp
)

add_library(${SC_INFRASTRUCTURE_TARGET}
    ${SC_INFRASTRUCTURE_SOURCES}
)


target_link_libraries(${SC_INFRASTRUCTURE_TARGET}
    PUBLIC
        Qt6::Sql
        Qt6::Core
        sc_application
)

target_include_directories(${SC_INFRASTRUCTURE_TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${SC_INFRASTRUCTURE_TARGET}
    PUBLIC cxx_std_20
)





