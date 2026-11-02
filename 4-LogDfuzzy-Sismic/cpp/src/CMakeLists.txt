add_library(fuzzy_core_lib
    aggregator.cpp
    defuzzifier.cpp
    fuzzy_rule.cpp
    fuzzy_system.cpp
    fuzzifier.cpp
    inference_engine.cpp
    linguistic_variable.cpp
    membership_function.cpp
    trapezoidal_mf.cpp
    triangular_mf.cpp
)

target_include_directories(fuzzy_core_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/cpp/include
)

target_link_libraries(fuzzy_core_lib
    PUBLIC Eigen3::Eigen
)