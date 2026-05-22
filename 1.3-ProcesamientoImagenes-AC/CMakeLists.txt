cmake_minimum_required(VERSION 3.16)
project(DetectorBordesAC VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 🔍 SFML 3 + vcpkg: componentes en MAYÚSCULA
find_package(SFML 3 REQUIRED COMPONENTS Graphics)

# 📦 Definir ejecutable (ANTES de cualquier target_*)
add_executable(detector_bordes
    src/main.cpp
    src/automata2d.cpp
    src/procesador_imagen.cpp
    src/detector_bordes.cpp
)

target_include_directories(detector_bordes PRIVATE ${CMAKE_SOURCE_DIR}/src)

# 🔗 Vincular con target moderno de SFML 3 (SFML::Graphics, NO sfml-graphics)
target_link_libraries(detector_bordes PRIVATE SFML::Graphics)

if(MSVC)
    target_compile_options(detector_bordes PRIVATE /W4 /utf-8)
endif()