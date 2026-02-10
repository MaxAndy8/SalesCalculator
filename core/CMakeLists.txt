add_library(sc_core INTERFACE)

target_link_libraries(sc_core
    INTERFACE
          sc_core_db
#         sc_core_domain
#         sc_core_application
)

target_include_directories(sc_core
    INTERFACE
        ${CMAKE_SOURCE_DIR}
)

# add_subdirectory(domain)
# add_subdirectory(application)
add_subdirectory(db)
