if(TARGET GTest::gtest_main)
    add_executable(
        week3_tue_task_d_iterator_invalidation_test
        tests/iterator_invalidation_test.cpp
    )

    target_compile_features(
        week3_tue_task_d_iterator_invalidation_test
        PRIVATE cxx_std_17
    )

    target_link_libraries(
        week3_tue_task_d_iterator_invalidation_test
        PRIVATE GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_tue_task_d_iterator_invalidation_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_tue_task_d_iterator_invalidation_test
        TEST_PREFIX "exercises.week3.tue.task_d."
        DISCOVERY_TIMEOUT 30
    )
endif()