cmake_minimum_required(VERSION 3.21)

project(SalesCalculator
    VERSION 0.1.0
    LANGUAGES CXX
)

# ================================
# Global CMake settings
# ================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ================================
# Qt
# ================================
set(QT_MIN_VERSION 6.5)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
    Sql
    LinguistTools
)

# Enable Qt auto features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ================================
# Global options
# ================================
option(SC_ENABLE_LOGGING "Enable logging" ON)
option(SC_ENABLE_TRANSLATIONS "Enable translations" ON)
option(SC_ENABLE_TESTS "Enable tests" OFF)

# # # ================================
# # # Project-wide include paths
# # # ================================
# # include_directories(
# #     ${CMAKE_SOURCE_DIR}
# # )

# ================================
# Subprojects (layers)
# ================================

add_subdirectory(core)
# # add_subdirectory(infrastructure)
add_subdirectory(ui)

# # # ================================
# # # Resources
# # # ================================
# # add_subdirectory(resources)

# # # ================================
# # # Translations
# # # ================================
# # if (SC_ENABLE_TRANSLATIONS)
# #     add_subdirectory(i18n)
# # endif()

# # # ================================
# # # Tests
# # # ================================
# # if (SC_ENABLE_TESTS)
# #     enable_testing()
# #     add_subdirectory(tests)
# # endif()

# # # ================================
# # # Install (future)
# # # ================================
# # install(TARGETS SalesCalculator
# #     RUNTIME DESTINATION bin
# # )










