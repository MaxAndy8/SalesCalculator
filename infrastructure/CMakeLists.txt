# =====================================
# Infrastructure layer
# DB, repositories, persistence
# =====================================

set(INFRASTRUCTURE_SOURCES

    # -------------------------------
    # Database
    # -------------------------------
    db/dbconnection.cpp
    db/dbconnection.h

    db/dbconfig.cpp
    db/dbconfig.h

    db/postgresconnection.cpp
    db/postgresconnection.h

    # -------------------------------
    # Repository base
    # -------------------------------
    repository/baserepository.cpp
    repository/baserepository.h

    # -------------------------------
    # Catalog repositories
    # -------------------------------
    repository/catalog/nomenclaturerepository.cpp
    repository/catalog/nomenclaturerepository.h

    # -------------------------------
    # Auth / security
    # -------------------------------
    auth/usercredentials.cpp
    auth/usercredentials.h

    auth/authrepository.cpp
    auth/authrepository.h
)

# -------------------------------------
# Static library
# -------------------------------------
add_library(sc_infrastructure STATIC
    ${INFRASTRUCTURE_SOURCES}
)

# -------------------------------------
# Qt
# -------------------------------------
target_link_libraries(sc_infrastructure
    PUBLIC
        Qt6::Core
        Qt6::Sql
)

# -------------------------------------
# PostgreSQL (libpq)
# -------------------------------------
find_package(PostgreSQL REQUIRED)

target_link_libraries(sc_infrastructure
    PRIVATE
        PostgreSQL::PostgreSQL
)

# -------------------------------------
# Internal dependencies
# -------------------------------------
target_link_libraries(sc_infrastructure
    PUBLIC
        sc_domain
)

# -------------------------------------
# Include paths
# -------------------------------------
target_include_directories(sc_infrastructure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# -------------------------------------
# Compile definitions
# -------------------------------------
target_compile_definitions(sc_infrastructure
    PRIVATE
        SC_INFRASTRUCTURE_LAYER
)

# -------------------------------------
# Warnings
# -------------------------------------
target_compile_options(sc_infrastructure
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# -------------------------------------
# Install
# -------------------------------------
install(TARGETS sc_infrastructure
    ARCHIVE DESTINATION lib
)
