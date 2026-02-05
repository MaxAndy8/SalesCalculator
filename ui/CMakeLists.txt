add_subdirectory(base)
add_subdirectory(catalogs)
add_subdirectory(documents)

add_library(ui INTERFACE)

target_link_libraries(ui
    INTERFACE
        ui_base
        ui_catalogs
        ui_documents
)
