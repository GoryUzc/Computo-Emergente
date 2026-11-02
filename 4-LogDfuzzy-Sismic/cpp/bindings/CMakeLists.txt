pybind11_add_module(fuzzy_core
    pybind_module.cpp
)

target_link_libraries(fuzzy_core
    PRIVATE fuzzy_core_lib
)


target_include_directories(fuzzy_core PRIVATE
    "C:/Users/User/AppData/Local/Programs/Python/Python312/include"
)

target_link_directories(fuzzy_core PRIVATE
    "C:/Users/User/AppData/Local/Programs/Python/Python312/libs"
)

add_custom_command(TARGET fuzzy_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${PROJECT_SOURCE_DIR}/workspace/python/fuzzy_ui"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:fuzzy_core>"
        "${PROJECT_SOURCE_DIR}/workspace/python/fuzzy_ui"
    COMMENT "Copying fuzzy_core module to the Python frontend"
)