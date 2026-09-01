add_library(
    week3_Thu_task_D_coordinate_conversion
    src/coordinate_conversions.cpp
)
target_include_directories(
    week3_Thu_task_D_coordinate_conversion
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(
    week3_Thu_task_D_coordinate_conversion
    PUBLIC
    Eigen3::Eigen
)
target_compile_features(
    week3_Thu_task_D_coordinate_conversion
    PUBLIC cxx_std_17
)
aerial_enable_warnings(
    week3_Thu_task_D_coordinate_conversion
)

if(TARGET GTest::gtest_main)
    add_executable(
        week3_Thu_task_D_coordinate_conversion_test
        tests/coordinate_conversions_test.cpp
    )
    target_link_libraries(
        week3_Thu_task_D_coordinate_conversion_test
        PRIVATE
            week3_Thu_task_D_coordinate_conversion
            GTest::gtest_main
    )
    aerial_enable_warnings(
        week3_Thu_task_D_coordinate_conversion_test
    )
    include(GoogleTest)
    gtest_discover_tests(
        week3_Thu_task_D_coordinate_conversion_test
        TEST_PREFIX "exercises.week3.Thu.task_D."
        DISCOVERY_TIMEOUT 30
    )
endif()
