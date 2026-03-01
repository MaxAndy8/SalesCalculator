set(SC_UI_RESOURCES_TARGET sc_ui_resources)

add_library(${SC_UI_RESOURCES_TARGET}
    icons.qrc
    resource.qrc
)

target_link_libraries(${SC_UI_RESOURCES_TARGET}
    PUBLIC
        Qt6::Core
)
