# 1. Створюємо бібліотеку
add_library(libApplication STATIC
    Application.h
    Application.cpp
)

# 2. Вмикаємо інструменти Qt для бібліотеки
set_target_properties(libApplication PROPERTIES
    AUTOMOC ON
    # AUTOUIC ON
    # AUTORCC ON
)

# 3. Налаштовуємо інклуди (це важливо для зручності в IDE)
target_include_directories(libApplication
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"      # папка app
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"      # тут лежать ui_*.h
)

# 4. Підключаємо Qt (PUBLIC, щоб тести теж бачили Qt)
target_link_libraries(libApplication
    PUBLIC
        libUiMainWindow
        Qt6::Widgets
)

# 5. Сам додаток стає дуже легким
add_executable(SalesCalculatorApp main.cpp)
target_link_libraries(SalesCalculatorApp PRIVATE
    libApplication
    #libUiMainWindow
    libUiBase
)

