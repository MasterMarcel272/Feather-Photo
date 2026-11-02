cmake_minimum_required(VERSION 3.21)

project(FeatherManager
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

if(WIN32)
    enable_language(RC)
endif()

add_executable(FeatherManager
    assets/FeatherManager.qrc
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

if(WIN32)
    target_sources(FeatherManager PRIVATE assets/FeatherManager.rc)
endif()

target_link_libraries(FeatherManager
    PRIVATE
        Qt6::Widgets
        Qt6::Network
)

install(TARGETS FeatherManager
    RUNTIME DESTINATION .
)
