cmake_minimum_required(VERSION 3.23)
project(LSystemModel)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR C:\\Users\\lenovo\\Desktop\\opencv\\build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(LSystemModel main.cpp)
target_link_libraries(LSystemModel ${OpenCV_LIBS})
