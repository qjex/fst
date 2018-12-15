cmake_minimum_required(VERSION 3.12)
project(fst)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#-fsanitize=address,undefined
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -g -fstandalone-debug")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(Qt5Widgets REQUIRED)

add_executable(fst main.cpp mainwindow.h mainwindow.cpp file_utils.cpp file_utils.h watcher.cpp watcher.h indexing_worker.cpp indexing_worker.h searching_worker.cpp searching_worker.h)

qt5_use_modules(fst Widgets)
include_directories("${CMAKE_BINARY_DIR}")
