cmake_minimum_required(VERSION 3.10)
project(Formas)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(main
    src/main.cpp
    src/procesamiento.cpp
)

target_link_libraries(main ${OpenCV_LIBS})