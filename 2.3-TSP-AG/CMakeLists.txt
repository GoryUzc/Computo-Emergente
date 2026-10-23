cmake_minimum_required(VERSION 3.16)
project(TSP_AG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" CACHE PATH "vcpkg root directory")
if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Ejecutable
add_executable(tsp_ag
    src/main.cpp
    src/tsp_core.cpp
    src/ag_core.cpp
    src/operadores.cpp
    src/exportador.cpp
)

target_include_directories(tsp_ag PRIVATE src)

# Optimizaciones
target_compile_options(tsp_ag PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)