cmake_minimum_required(VERSION 3.16)
project(calculator_example VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Sources for the calculator example
set(SOURCES
    CalculatorWindow.cpp
    CalculatorSubWindow.cpp
    main.cpp
    CalculatorNodeItemBase.cpp
    DragListWidget.cpp
    NodeSelectorPopupWidget.cpp
)

set(HEADERS
    CalculatorWindow.h
    CalculatorSubWindow.h
    CalculatorNodeItemBase.h
    CalculatorNodes.h
    DragListWidget.h
    NodeSelectorPopupWidget.h
    CalculatorNodeEditorGraphicsView.h
)


add_executable(calculator ${SOURCES} ${HEADERS} ${RESOURCES})

# Link to node-editor (do NOT redefine)
target_link_libraries(calculator PRIVATE node-editor Qt6::Widgets)
target_include_directories(calculator PRIVATE ../../include)
