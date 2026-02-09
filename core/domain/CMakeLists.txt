add_library(sc_domain STATIC
    common/id.h
    common/version.h
    common/objectreference.h
    common/baseentity.h

    catalog/nomenclature.h
    catalog/counterparty.h
    catalog/catalogevents.h

    document/goodsreceipt.h

    auth/user.h
)

target_include_directories(sc_domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
