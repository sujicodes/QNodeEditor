cmake_minimum_required(VERSION 3.16)

set(APP_GLOBAL_VERSION "1.0.0" CACHE STRING "Version string")
set(APP_GLOBAL_NAME calculator)

project(${APP_GLOBAL_NAME} VERSION ${APP_GLOBAL_VERSION} LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

add_subdirectory(calculator)
