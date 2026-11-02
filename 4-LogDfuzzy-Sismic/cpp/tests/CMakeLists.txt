FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(googletest)

add_executable(test_fuzzy_system test_fuzzy_system.cpp)

target_link_libraries(test_fuzzy_system PRIVATE
    fuzzy_core_lib
    GTest::gtest_main
)

add_test(NAME fuzzy_system COMMAND test_fuzzy_system)
