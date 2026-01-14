set(SOURCES
    CalculatorWindow.cpp
    CalculatorSubWindow.cpp
    main.cpp
    CalculatorConfig.h
)

add_executable(calculator ${SOURCES}
    CalculatorConfig.h
    CalculatorGraphicsNode.h CalculatorGraphicsNode.cpp)

target_include_directories(calculator PRIVATE
    ${CMAKE_SOURCE_DIR}        # <-- gives access to the base node editor headers
)

target_link_libraries(calculator PRIVATE
    nodeeditor
    Qt${QT_VERSION_MAJOR}::Widgets
)
