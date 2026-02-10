add_library(sc_core_db STATIC
    DatabaseConnectionInfo.h
    # DatabaseConnectionManager.cpp
)

target_include_directories(sc_core_db
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
