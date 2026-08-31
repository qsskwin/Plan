add_library(week3_wed_task_b_controller
    src/controller.cpp
)

target_include_directories(week3_wed_task_b_controller PUBLIC include)

target_compile_features(
    week3_wed_task_b_controller
    PUBLIC cxx_std_17
)

aerial_enable_warnings(
    week3_wed_task_b_controller
)

add_executable(
    week3_wed_task_b_controller_demo
    app/controller_demo.cpp
)

aerial_enable_warnings(
    week3_wed_task_b_controller_demo
)

target_link_libraries(
    week3_wed_task_b_controller_demo
    PRIVATE week3_wed_task_b_controller
)

if (TARGET GTest::gtest_main)
    add_executable(
        week3_wed_task_b_controller_test
        tests/controller_test.cpp
    )

    target_link_libraries(
        week3_wed_task_b_controller_test
        PRIVATE
            week3_wed_task_b_controller
            GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_wed_task_b_controller_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_wed_task_b_controller_test
        TEST_PREFIX "exercises.week3.wed.task_b."
        DISCOVERY_TIMEOUT 30
    )
endif()

