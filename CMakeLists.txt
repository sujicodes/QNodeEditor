# cmake_minimum_required(VERSION 3.16)

# project(nodeEditor VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# set(PROJECT_SOURCES
#         main.cpp
# )

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     qt_add_executable(nodeEditor
#         MANUAL_FINALIZATION
#         ${PROJECT_SOURCES}
#         NodeEditorWindow.cpp
#         NodeEditorWindow.h
#         NodeEditorWidget.cpp
#         NodeEditorWidget.h
#         NodeGraphicsScene.cpp
#         NodeGraphicsScene.h
#         NodeEditorGraphicsView.cpp
#         NodeEditorGraphicsView.h
#         Scene.cpp
#         Scene.h
#         NodeGraphicsItem.cpp
#         NodeGraphicsItem.h
#         Node.cpp
#         Node.h
#         Socket.cpp
#         Socket.h
#         SocketGraphicsItem.cpp
#         SocketGraphicsItem.h
#         Edge.cpp
#         Edge.h
#         EdgeGraphicsPathItem.cpp
#         EdgeGraphicsPathItem.h
#         BezierEdgeGraphicsPathItem.cpp
#         BezierEdgeGraphicsPathItem.h
#         DirectEdgeGraphicsPathItem.cpp
#         DirectEdgeGraphicsPathItem.h
#         history.cpp
#         history.h
#         Serializable.h
#         UndoCommands.h
#         Theme.h
#         NodeSelectorWidget.h
#         NodeSelectorWidget.cpp
#         examples/calculator/CalculatorSubWindow.h examples/calculator/CalculatorSubWindow.cpp
#     )
# # Define target properties for Android with Qt 6 as:
# #    set_property(TARGET nodeEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# else()
#     if(ANDROID)
#         add_library(nodeEditor SHARED
#             ${PROJECT_SOURCES}
#         )
# # Define properties for Android with Qt 5 after find_package() calls as:
# #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#     else()
#         add_executable(nodeEditor
#             ${PROJECT_SOURCES}
#         )
#     endif()
# endif()

# target_link_libraries(nodeEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# # If you are developing for iOS or macOS you should consider setting an
# # explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.nodeEditor)
# endif()
# set_target_properties(nodeEditor PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

# include(GNUInstallDirs)
# install(TARGETS nodeEditor
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(nodeEditor)
# endif()


cmake_minimum_required(VERSION 3.16)
project(nodeEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# -------------------------
# Core Node Editor Library
# -------------------------
add_library(nodeeditor STATIC
    NodeEditorWindow.cpp
    NodeEditorWindow.h
    NodeEditorWidget.cpp
    NodeEditorWidget.h
    NodeGraphicsScene.cpp
    NodeGraphicsScene.h
    NodeEditorGraphicsView.cpp
    NodeEditorGraphicsView.h
    NodeSelectorWidget.h
    NodeSelectorWidget.cpp
    Scene.cpp
    Scene.h
    NodeGraphicsItem.cpp
    NodeGraphicsItem.h
    Node.cpp
    Node.h
    Socket.cpp
    Socket.h
    SocketGraphicsItem.cpp
    SocketGraphicsItem.h
    Edge.cpp
    Edge.h
    EdgeGraphicsPathItem.cpp
    EdgeGraphicsPathItem.h
    BezierEdgeGraphicsPathItem.cpp
    BezierEdgeGraphicsPathItem.h
    DirectEdgeGraphicsPathItem.cpp
    DirectEdgeGraphicsPathItem.h
    history.cpp
    history.h
    Serializable.h
    UndoCommands.h
    Theme.h
    DragListWidget.h DragListWidget.cpp
    icons.qrc
)

target_link_libraries(nodeeditor PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(nodeeditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------
# Examples
# -------------------------
add_subdirectory(examples)
