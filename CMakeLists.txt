cmake_minimum_required(VERSION 3.21)
project(SalesCalculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

#add_subdirectory(core)
#add_subdirectory(infrastructure)
#add_subdirectory(ui)
add_subdirectory(app)








