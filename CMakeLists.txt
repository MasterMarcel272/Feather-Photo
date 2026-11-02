cmake_minimum_required(VERSION 3.21)

project(FeatherPhoto
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

if(WIN32)
    enable_language(RC)
endif()

add_executable(FeatherPhoto
    assets/FeatherPhoto.qrc
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h

    src/Layer.cpp
    src/Layer.h

    src/Document.cpp
    src/Document.h

    src/CanvasWidget.cpp
    src/CanvasWidget.h

    src/BrushTool.cpp
    src/BrushTool.h
)

if(WIN32)
    target_sources(FeatherPhoto PRIVATE assets/FeatherPhoto.rc)
endif()

target_link_libraries(FeatherPhoto
    PRIVATE
        Qt6::Widgets
)

install(TARGETS FeatherPhoto
    RUNTIME DESTINATION .
)
