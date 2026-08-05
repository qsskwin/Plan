# Week1/Wed/task_A 是根工程的子目录，不单独声明 project()。

add_library(week1_wed_task_a_lifecycle
    src/lifecycle_raii.cpp
)

target_include_directories(week1_wed_task_a_lifecycle
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(week1_wed_task_a_lifecycle PUBLIC cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        week1_wed_task_a_lifecycle
        PRIVATE -Wall -Wextra -Wpedantic
    )
endif()

add_executable(week1_wed_task_a_demo
    apps/demo.cpp
)

target_link_libraries(
    week1_wed_task_a_demo
    PRIVATE week1_wed_task_a_lifecycle
)

if(BUILD_TESTING)
    add_executable(week1_wed_task_a_tests
        tests/lifecycle_raii_test.cpp
    )

    target_link_libraries(
        week1_wed_task_a_tests
        PRIVATE week1_wed_task_a_lifecycle
    )

    add_test(
        NAME exercises.week1.wed.task_a
        COMMAND week1_wed_task_a_tests
    )
endif()
