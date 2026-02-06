# 1. Створюємо бібліотеку
add_library(libUiMainWindow STATIC
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
)

# 2. Вмикаємо інструменти Qt для бібліотеки
set_target_properties(libUiMainWindow PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    # AUTORCC ON
)

# 3. Налаштовуємо інклуди (це важливо для зручності в IDE)
target_include_directories(libUiMainWindow
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"       # папка ui
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"       # тут лежать ui_*.h
)

# 4. Підключаємо Qt (PUBLIC, щоб тести теж бачили Qt)
target_link_libraries(libUiMainWindow PUBLIC
    Qt6::Widgets)



add_subdirectory(base)
# add_subdirectory(catalogs)
# add_subdirectory(documents)

add_library(ui INTERFACE)

target_link_libraries(ui
    INTERFACE        
        libUiMainWindow
        libUiBase
)
