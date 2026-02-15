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

# Усі бібліотеки будуються як position-independent (важливо для shared libs)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)

# ================================
# Global options
# ================================
option(SC_ENABLE_TESTS "Enable tests" OFF)

# ================================
# Qt
# ================================
set(QT_MIN_VERSION 6.5)

# Основні Qt компоненти
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
    Sql
    LinguistTools
)

# ================================
# Qt Auto Features
# ================================
set(CMAKE_AUTOMOC ON)    # MOC (signals/slots)
set(CMAKE_AUTOUIC ON)    # Обробка .ui файлів
set(CMAKE_AUTORCC ON)    # Обробка .qrc ресурсів

# ================================
# Project structure (Clean Architecture)
# ================================

# ==============================
# 🔵 Domain layer (Enterprise Business Rules)
# - Чиста бізнес-логіка
# - Не залежить ні від чого
# ==============================
add_subdirectory(src/domain)

# ==============================
# 🟢 Application layer (Use Cases)
# - Оркестрація бізнес-логіки
# - Залежить від domain
# ==============================
add_subdirectory(src/application)

# ==============================
# 🟠 Infrastructure layer (Frameworks & Drivers)
# - БД, файлові системи, мережа
# - Реалізації репозиторіїв
# - Залежить від domain
# ==============================
add_subdirectory(src/infrastructure)

# ==============================
# 🟣 Core / Shared kernel
# - Загальні утиліти
# - Logger, AppContext, helpers
# - Може залежати від domain
# ==============================
add_subdirectory(src/core)

# ==============================
# 🟡 UI Resources (Presentation support)
# - Qt .qrc
# - Іконки, стилі, картинки
# - Належить до Presentation layer
# ==============================
add_subdirectory(src/ui_resources)

# ==============================
# 🟡 UI i18n (Presentation support)
# - .ts / .qm переклади
# - Належить до Presentation layer
# ==============================
add_subdirectory(src/ui_i18n)

# ==============================
# 🔷 UI layer (Presentation)
# - Вікна, діалоги, віджети
# - Залежить від application + core
# ==============================
add_subdirectory(src/ui)

# ==============================
# 🔴 Composition Root / App layer
# - main()
# - Збирання всіх залежностей
# - Створення runtime
# ==============================
add_subdirectory(src/app)

# ================================
# Tests (optional)
# ================================
if (SC_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
