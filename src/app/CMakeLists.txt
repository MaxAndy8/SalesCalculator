# ==============================
# 🔴 Composition Root / App layer
# ==============================

set(SC_APP_TARGET sc_app)

add_executable(${SC_APP_TARGET}
    main.cpp
)

target_link_libraries(${SC_APP_TARGET}
    PRIVATE
        Qt6::Core
        Qt6::Widgets

        # # sc_domain
        # # sc_application
        # # sc_infrastructure
        sc_core
        sc_ui
        # # sc_ui_resources
        # # sc_ui_i18n
)

target_include_directories(${SC_APP_TARGET}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(${SC_APP_TARGET} PROPERTIES
    OUTPUT_NAME "SalesCalculator"
)
