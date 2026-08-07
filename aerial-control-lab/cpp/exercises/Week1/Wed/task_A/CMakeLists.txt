# Week1/Wed/task_A 是根工程的子目录，不单独声明 project()。

# 把生命周期观察器和 ScopeTimer 的实现编译成静态/默认类型库，供演示程序和测试复用。
add_library(week1_wed_task_a_lifecycle
    src/lifecycle_raii.cpp
)

# PUBLIC 表示：当前库编译时需要该头文件目录，链接本库的下游目标也会自动继承它。
target_include_directories(week1_wed_task_a_lifecycle
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 本练习使用 std::make_unique、移动语义等现代 C++ 功能，并统一要求至少 C++17。
# PUBLIC 会把该语言标准要求一并传递给链接此库的演示程序和测试程序。
target_compile_features(week1_wed_task_a_lifecycle PUBLIC cxx_std_17)

# 只为 GCC/Clang 系列编译器启用对应的严格警告参数，避免把不兼容选项传给 MSVC。
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        week1_wed_task_a_lifecycle
        PRIVATE -Wall -Wextra -Wpedantic
    )
endif()

# 构建可手工运行的示例程序，用于观察各阶段计数、所有权转移和计时输出。
add_executable(week1_wed_task_a_demo
    apps/demo.cpp
)

# PRIVATE 表示链接关系只属于 demo，不需要继续传递给其他下游目标。
target_link_libraries(
    week1_wed_task_a_demo
    PRIVATE week1_wed_task_a_lifecycle
)

# 只有根工程启用 CTest/BUILD_TESTING 时才生成测试可执行文件并注册测试，
# 让使用者可以在不需要测试的构建中减少编译目标。
if(BUILD_TESTING)
    add_executable(week1_wed_task_a_tests
        tests/lifecycle_raii_test.cpp
    )

    # 测试通过公开头文件调用被测库，与实际使用方式保持一致。
    target_link_libraries(
        week1_wed_task_a_tests
        PRIVATE week1_wed_task_a_lifecycle
    )

    # 向 CTest 注册稳定的测试名称；COMMAND 对应上面生成的测试可执行目标。
    add_test(
        NAME exercises.week1.wed.task_a
        COMMAND week1_wed_task_a_tests
    )
endif()
