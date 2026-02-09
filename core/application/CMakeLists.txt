add_library(sc_application STATIC
    catalog/nomenclature_service.cpp
    catalog/counterparty_service.cpp

    document/goodsreceipt_service.cpp

    auth/auth_service.cpp
)

target_include_directories(sc_application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sc_application PUBLIC sc_domain)
