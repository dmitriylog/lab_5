cmake_minimum_required(VERSION 3.16)
project(Calculator)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(calc Calculator.cpp)

target_link_libraries(calc Qt6::Widgets)
