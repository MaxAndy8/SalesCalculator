# ==============================
# 🟢 Application layer
# - Use cases
# - DTO
# - Coordination logic
# - Не залежить від UI
# ==============================

set(SC_APPLICATION_TARGET sc_application)

set(SC_APPLICATION_SOURCES
    database/DatabaseConnectionInfo.h
    auth/AuthenticatedUser.h
    auth/AuthenticatedUser.cpp
    auth/IAuthService.h
)

add_library(${SC_APPLICATION_TARGET}
    ${SC_APPLICATION_SOURCES}
)

target_link_libraries(${SC_APPLICATION_TARGET}
    PUBLIC
        Qt6::Core
        # # sc_domain
)

target_include_directories(${SC_APPLICATION_TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${SC_APPLICATION_TARGET}
    PUBLIC cxx_std_20
)
