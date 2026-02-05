add_subdirectory(nomenclature)
add_subdirectory(counterparties)

add_library(ui_catalogs INTERFACE)

target_link_libraries(ui_catalogs
    INTERFACE
        ui_nomenclature
        ui_counterparties
)
