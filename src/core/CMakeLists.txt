# ==============================
# 🟣 Core / Shared kernel
# - Загальні утиліти
# - Logger, AppContext, helpers
# - Може використовуватись усіма шарами
# - Не залежить від UI
# ==============================

set(SC_CORE_TARGET sc_core)

set(SC_CORE_SOURCES
    Logger.cpp
    Logger.h

    # # AppContext.cpp
    # # AppContext.h
)

add_library(${SC_CORE_TARGET}
    ${SC_CORE_SOURCES}
)

target_link_libraries(${SC_CORE_TARGET}
    PUBLIC
        Qt6::Core
)

target_include_directories(${SC_CORE_TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(${SC_CORE_TARGET}
    PUBLIC cxx_std_20
)
