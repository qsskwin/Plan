add_library(
    week3_mon_task_c_sample_buffer
    src/sample_buffer.cpp
)

target_include_directories(
    week3_mon_task_c_sample_buffer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    week3_mon_task_c_sample_buffer
    PUBLIC cxx_std_17
)

aerial_enable_warnings(
    week3_mon_task_c_sample_buffer
)

if(TARGET GTest::gtest_main)
    add_executable(
        week3_mon_task_c_sample_buffer_test
        tests/sample_buffer_test.cpp
    )

    target_link_libraries(
        week3_mon_task_c_sample_buffer_test
        PRIVATE
            week3_mon_task_c_sample_buffer
            GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_mon_task_c_sample_buffer_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_mon_task_c_sample_buffer_test
        TEST_PREFIX "exercises.week3.mon.task_c."
        DISCOVERY_TIMEOUT 30
    )
endif()