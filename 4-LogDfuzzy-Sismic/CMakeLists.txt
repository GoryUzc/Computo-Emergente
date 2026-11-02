cmake_minimum_required(VERSION 3.20)
project(FuzzySismic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

set(VCPKG_APPLOCAL_DEPS OFF)
find_package(Eigen3 CONFIG REQUIRED)

include(FetchContent)

FetchContent_Declare(pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG master
)
FetchContent_MakeAvailable(pybind11)

add_subdirectory(cpp/src)
add_subdirectory(cpp/bindings)
add_subdirectory(cpp/tests)

install(TARGETS fuzzy_core
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)