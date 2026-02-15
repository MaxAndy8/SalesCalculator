# ==============================
# 🔷 UI layer (Presentation)
# - Вікна, діалоги, віджети
# - Залежить від application + core
# ==============================

set(SC_UI_TARGET sc_ui)

set(SC_UI_SOURCES
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui

    application/ScApplication.cpp
    application/ScApplication.h

    dialogs/SelectDatabaseDialog.cpp
    dialogs/SelectDatabaseDialog.h
    dialogs/SelectDatabaseDialog.ui

    dialogs/EditDatabaseDialog.cpp
    dialogs/EditDatabaseDialog.h
    dialogs/EditDatabaseDialog.ui

    # # dialogs/AuthDialog.cpp
    # # dialogs/AuthDialog.h
)

add_library(${SC_UI_TARGET}
    ${SC_UI_SOURCES}
)

target_link_libraries(${SC_UI_TARGET}
    PUBLIC
        Qt6::Core
        Qt6::Widgets

        # # sc_application
        sc_core

        # # sc_ui_resources
        # # sc_ui_i18n
)

target_include_directories(${SC_UI_TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${SC_UI_TARGET}
    PUBLIC cxx_std_20
)

