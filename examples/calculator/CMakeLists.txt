set(SOURCES
    CalculatorWindow.cpp
    CalculatorSubWindow.cpp
    CalculatorGraphicsNode.cpp
    main.cpp
    CalculatorConfig.h
    CalculatorNodeBase.cpp
    CalculatorNodes.h
    CalculatorNodes.cpp
)

add_executable(calculator ${SOURCES})

target_include_directories(calculator PRIVATE
    ${CMAKE_SOURCE_DIR}        # <-- gives access to the base node editor headers
)

target_link_libraries(calculator PRIVATE
    nodeeditor
    Qt${QT_VERSION_MAJOR}::Widgets
)

install(TARGETS calculator
        RUNTIME DESTINATION bin)
