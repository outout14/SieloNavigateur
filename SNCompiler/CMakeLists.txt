cmake_minimum_required(VERSION 3.6)
project(sielo-compiler)

include_directories(${CMAKE_SOURCE_DIR}/SNCompiler)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(
        GLOB_RECURSE
        SOURCE_FILES
        Main.cpp
        Application.hpp
        Application.cpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

add_executable(sielo-compiler ${SOURCE_FILES})
target_link_libraries(sielo-compiler Qt5::Widgets)
target_link_libraries(sielo-compiler Qt5::WebEngine)
target_link_libraries(sielo-compiler Qt5::WebEngineWidgets)