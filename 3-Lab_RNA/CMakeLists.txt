cmake_minimum_required(VERSION 3.16)
project(Lab_RNA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VCPKG_APPLOCAL_DEPS OFF)


find_package(Eigen3 CONFIG REQUIRED)

add_executable(Lab_RNA
    src/main.cpp
    src/mcculloch_perceptron.cpp
    src/adaline.cpp
    src/mlp_backprop.cpp
    src/kohonen_som.cpp
    src/hopfield.cpp
)

target_include_directories(Lab_RNA PRIVATE src include)
target_link_libraries(Lab_RNA PRIVATE Eigen3::Eigen)

if(MSVC)
    target_compile_options(Lab_RNA PRIVATE /W4 /O2 /fp:fast /permissive-)
else()
    target_compile_options(Lab_RNA PRIVATE -Wall -Wextra -O3 -ffast-math -march=native)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python")
