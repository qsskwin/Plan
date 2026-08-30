add_library(
    week3_tue_task_b_stl_containers
    src/stl_containers.cpp
)

target_include_directories(
    week3_tue_task_b_stl_containers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    week3_tue_task_b_stl_containers
    PUBLIC cxx_std_17
)

aerial_enable_warnings(
    week3_tue_task_b_stl_containers
)

if(TARGET GTest::gtest_main)
    add_executable(
        week3_tue_task_b_stl_containers_test
        tests/stl_containers_test.cpp
    )

    target_link_libraries(
        week3_tue_task_b_stl_containers_test
        PRIVATE
            week3_tue_task_b_stl_containers
            GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_tue_task_b_stl_containers_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_tue_task_b_stl_containers_test
        TEST_PREFIX "exercises.week3.tue.task_b."
        DISCOVERY_TIMEOUT 30
    )
endif()

