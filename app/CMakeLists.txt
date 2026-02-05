# Вмикаємо автоматичну генерацію коду для Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Додаємо шлях до папки ui, щоб компілятор бачив згенеровані файли
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ui")

add_executable(SalesCalculatorApp main.cpp)

# Додаємо файли окремо, щоб CMake краще їх підхопив
target_sources(SalesCalculatorApp PRIVATE
    SCApplication.h
    SCApplication.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../ui/MainWindow.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ui/MainWindow.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ui/MainWindow.cpp"
)

target_link_libraries(SalesCalculatorApp PRIVATE
    Qt6::Widgets
)
