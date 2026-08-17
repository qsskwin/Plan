add_library(week2_mon_control_utils src/control_utils.cpp)

target_include_directories(week2_mon_control_utils PUBLIC include)

target_link_libraries(week2_mon_control_utils PUBLIC Eigen3::Eigen)

add_executable(week2_mon_control_utils_demo app/control_utils_demo.cpp)

target_link_libraries(week2_mon_control_utils_demo PRIVATE week2_mon_control_utils)

if(TARGET GTest::gtest_main)
    add_executable(week2_mon_control_utils_test tests/control_utils_test.cpp)

    target_link_libraries(week2_mon_control_utils_test PRIVATE week2_mon_control_utils GTest::gtest_main)

    include(GoogleTest)

    gtest_discover_tests(week2_mon_control_utils_test)
endif()

