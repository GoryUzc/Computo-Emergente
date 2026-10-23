cmake_minimum_required(VERSION 3.16)

project(ags_maximizador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ags_maximizador
    src/main.cpp
    src/ag_core.cpp
    src/operadores.cpp
    src/utils.cpp
    src/exportador.cpp
)
