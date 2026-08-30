add_library(
    week3_tue_task_c_algorithm_exercises
    src/algorithm_exercises.cpp
)

target_compile_features(
    week3_tue_task_c_algorithm_exercises
    PUBLIC cxx_std_17
)

target_include_directories(
    week3_tue_task_c_algorithm_exercises
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

aerial_enable_warnings(
    week3_tue_task_c_algorithm_exercises
)

if (TARGET GTest::gtest_main)
    add_executable(
        week3_tue_task_c_algorithm_exercises_test
        tests/algorithm_exercises_test.cpp
    )

    target_link_libraries(
        week3_tue_task_c_algorithm_exercises_test
        PRIVATE
            week3_tue_task_c_algorithm_exercises
            GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_tue_task_c_algorithm_exercises_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_tue_task_c_algorithm_exercises_test
        TEST_PREFIX "exercises.week3.tue.task_c."
        DISCOVERY_TIMEOUT 30
    )
endif()
