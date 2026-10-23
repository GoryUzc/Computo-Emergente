cmake_minimum_required(VERSION 3.16)
project(AG_Bohachevsky)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/ag_core.cpp
    src/operadores.cpp
    src/renormalizacion.cpp
    src/exportador.cpp
)

add_executable(ag_bohachevsky ${SOURCES})

target_include_directories(ag_bohachevsky PRIVATE ${CMAKE_SOURCE_DIR}/src)