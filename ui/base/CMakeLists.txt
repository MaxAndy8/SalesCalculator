# 1. Створюємо бібліотеку
add_library(libUiBase STATIC

    BaseCatalogListForm.h
    BaseCatalogListForm.cpp
    BaseCatalogListForm.ui

    BaseCatalogSelectForm.h
    BaseCatalogSelectForm.cpp
    BaseCatalogSelectForm.ui

    BaseCatalogObjectForm.h
    BaseCatalogObjectForm.cpp
    BaseCatalogObjectForm.ui

)

# 2. Вмикаємо всі інструменти Qt для бібліотеки
set_target_properties(libUiBase PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    # AUTORCC ON
)

# 3. Налаштовуємо інклуди (це важливо для зручності в IDE)
target_include_directories(libUiBase
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"       # папка ui/base
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"       # тут лежать ui_*.h
)

# 4. Підключаємо Qt (PUBLIC, щоб тести теж бачили Qt)
target_link_libraries(libUiBase PUBLIC
    Qt6::Widgets)

