add_library(week2_fri_free_fall
    src/free_fall_integrators.cpp
)

target_include_directories(week2_fri_free_fall
    PUBLIC
        include
)

target_link_libraries(week2_fri_free_fall
    PUBLIC
        Eigen3::Eigen
)

target_compile_features(week2_fri_free_fall
    PUBLIC
        cxx_std_17
)

add_executable(week2_fri_free_fall_demo
    app/free_fall_demo.cpp
)

target_link_libraries(week2_fri_free_fall_demo
    PRIVATE
        week2_fri_free_fall
)

if(TARGET GTest::gtest_main)
    add_executable(week2_fri_free_fall_test
        tests/free_fall_integrators_test.cpp
    )

    target_link_libraries(week2_fri_free_fall_test
        PRIVATE
            week2_fri_free_fall
            GTest::gtest_main
    )

    include(GoogleTest)

    gtest_discover_tests(
        week2_fri_free_fall_test
        DISCOVERY_TIMEOUT 30
    )    


endif()