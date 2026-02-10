# =====================================
# UI layer – executable
# =====================================

set(UI_SOURCES
    main.cpp

    # Application
    application/ScApplication.h
    application/ScApplication.cpp

    # Main window
    windows/MainWindow.cpp
    windows/MainWindow.h
    windows/MainWindow.ui

    # Dialogs
    dialogs/SelectDatabaseDialog.cpp
    dialogs/SelectDatabaseDialog.h
    dialogs/SelectDatabaseDialog.ui

    # EditDatabaseDialog
    dialogs/EditDatabaseDialog.cpp
    dialogs/EditDatabaseDialog.h
    dialogs/EditDatabaseDialog.ui

    # # dialogs/authdialog.cpp
    # # dialogs/authdialog.h
    # # dialogs/authdialog.ui

    # # # Catalog forms (example)
    # # forms/catalog/basecataloglistform.cpp
    # # forms/catalog/basecataloglistform.h
    # # forms/catalog/basecataloglistform.ui

    # # forms/catalog/basecatalogobjectform.cpp
    # # forms/catalog/basecatalogobjectform.h
    # # forms/catalog/basecatalogobjectform.ui

    # # forms/catalog/nomenclaturelistform.cpp
    # # forms/catalog/nomenclaturelistform.h
    # # forms/catalog/nomenclaturelistform.ui

    # # forms/catalog/nomenclatureobjectform.cpp
    # # forms/catalog/nomenclatureobjectform.h
    # # forms/catalog/nomenclatureobjectform.ui
)

add_executable(SalesCalculator
    ${UI_SOURCES}    
)

# -------------------------------------
# Qt
# -------------------------------------
target_link_libraries(SalesCalculator
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)

# -------------------------------------
# Internal dependencies
# -------------------------------------
target_link_libraries(SalesCalculator
     PRIVATE
        sc_core
    # # sc_application
    # # sc_domain
    # # sc_infrastructure
)

# -------------------------------------
# Include paths
# -------------------------------------
target_include_directories(SalesCalculator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# # # -------------------------------------
# # # Compile options
# # # -------------------------------------
# # target_compile_definitions(SalesCalculator
# #     PRIVATE
# #         SC_UI_LAYER
# # )

# # # -------------------------------------
# # # Resources
# # # -------------------------------------
# # # (icons, styles, etc.)
# # # Handled via AUTORCC if .qrc files are added

# # # -------------------------------------
# # # Install
# # # -------------------------------------
# # install(TARGETS SalesCalculator
# #     RUNTIME DESTINATION bin
# # )
