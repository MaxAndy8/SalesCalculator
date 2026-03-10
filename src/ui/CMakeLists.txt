# ==============================
# 🔷 UI layer (Presentation)
# - Вікна, діалоги, віджети
# - Залежить від application + core
# ==============================

set(SC_UI_TARGET sc_ui)

set(SC_UI_SOURCES
    # Application
    application/ScApplication.cpp
    application/ScApplication.h

    # Dialogs
    dialogs/SelectDatabaseDialog.cpp
    dialogs/SelectDatabaseDialog.h
    dialogs/SelectDatabaseDialog.ui
    #
    dialogs/EditDatabaseDialog.cpp
    dialogs/EditDatabaseDialog.h
    dialogs/EditDatabaseDialog.ui
    #
    dialogs/AuthDialog.cpp
    dialogs/AuthDialog.h
    dialogs/AuthDialog.ui

    # MainWindow
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui

    widgets/UniversalReferenceWidget.cpp
    widgets/UniversalReferenceWidget.h
    widgets/UniversalReferenceWidget.ui
    #
    formController/FormType.h
    formController/IFormController.h
    #
    forms/base/ListForm.cpp
    forms/base/ListForm.h
    forms/base/ListForm.ui
    #
    forms/base/FormSyncBus.cpp
    forms/base/FormSyncBus.h
    #
    forms/base/MdiUtils.cpp
    forms/base/MdiUtils.h

    # CATALOGS:
    # Nomenclature
    forms/catalogs/nomenclature/NomenclatureItemForm.cpp
    forms/catalogs/nomenclature/NomenclatureItemForm.h
    forms/catalogs/nomenclature/NomenclatureItemForm.ui
    #
    forms/catalogs/nomenclature/NomenclatureListForm.cpp
    forms/catalogs/nomenclature/NomenclatureListForm.h
    #
    forms/catalogs/nomenclature/NomenclatureChoiceForm.cpp
    forms/catalogs/nomenclature/NomenclatureChoiceForm.h
    #
    forms/catalogs/nomenclature/NomenclatureGroupForm.cpp
    forms/catalogs/nomenclature/NomenclatureGroupForm.h
    forms/catalogs/nomenclature/NomenclatureGroupForm.ui
    #
    forms/catalogs/nomenclature/NomenclatureTreeModel.cpp
    forms/catalogs/nomenclature/NomenclatureTreeModel.h

    # Unit
    forms/catalogs/units/UnitChoiceForm.cpp
    forms/catalogs/units/UnitChoiceForm.h
    #
    forms/catalogs/units/UnitItemForm.cpp
    forms/catalogs/units/UnitItemForm.h
    forms/catalogs/units/UnitItemForm.ui

)

add_library(${SC_UI_TARGET}
    ${SC_UI_SOURCES}
)

target_link_libraries(${SC_UI_TARGET}
    PUBLIC
        Qt6::Core
        Qt6::Widgets

        sc_application
        sc_core

        sc_ui_resources
        # # sc_ui_i18n
)

target_include_directories(${SC_UI_TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${SC_UI_TARGET}
    PUBLIC cxx_std_20
)

