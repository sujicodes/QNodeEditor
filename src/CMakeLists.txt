cmake_minimum_required(VERSION 3.16)
project(node_editor_lib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Source files
file(GLOB SRC_FILES
    "*.cpp"
)

file(GLOB HEADER_FILES
    "../include/*.h"
)

# If you have a resources file, include it here
set(RESOURCES ../resources.qrc)

# Only one add_library here
add_library(node-editor STATIC ${SRC_FILES} ${HEADER_FILES} ${RESOURCES})

target_link_libraries(node-editor PUBLIC Qt6::Widgets)
target_include_directories(node-editor PUBLIC ../include)
