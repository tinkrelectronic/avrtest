cmake_minimum_required(VERSION 3.16)
project(QtAvrdudess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_executable(QtAvrdudess
    main.cpp
    mainwindow.cpp
    mainwindow.h
    avrdude_conf_parser.cpp
    avrdude_conf_parser.h
)

target_link_libraries(QtAvrdudess PRIVATE Qt6::Core Qt6::Widgets)