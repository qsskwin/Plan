# 本文件不是独立 CMake 工程的入口，因此不重复调用 cmake_minimum_required() 和 project()。
# 配置 aerial-control-lab 时，根 CMakeLists.txt 通过下面的命令进入本目录：
#
#   add_subdirectory(cpp/exercises/Week1/Tue/task_A)
#
# 进入后，CMAKE_CURRENT_SOURCE_DIR 指向当前 task_A 源码目录；CMake 同时在构建目录中
# 创建对应的 build/cpp/exercises/Week1/Tue/task_A，用于保存本练习的中间文件和产物。

# 创建 Week1/Tue/task_A 的基础练习库，并把实际实现文件加入该 target。
# target 名称包含周、星期和任务编号，避免未来的 Week2、Mon 或 task_B 使用相同名称。
#
# 这里没有显式指定 STATIC 或 SHARED，因此库类型遵循父工程的 BUILD_SHARED_LIBS 设置；
# 默认情况下 BUILD_SHARED_LIBS 未开启，通常会生成静态库 libweek1_tue_task_a_basics.a。
# 练习代码单独组成这个库，不会被加入正式的 aerial_core 库。
add_library(week1_tue_task_a_basics
    src/cpp_basics.cpp
)

# 为练习库设置头文件搜索目录，使源码可以使用：
#
#   #include "task_a/cpp_basics.hpp"
#
# CMAKE_CURRENT_SOURCE_DIR 在本文件中就是 task_A 目录，所以实际搜索路径为 task_A/include。
# PUBLIC 表示该路径既用于编译练习库本身，也会自动传递给链接该库的 demo 和 tests，
# 因此两个可执行目标不需要再次设置相同的 include 路径。
target_include_directories(week1_tue_task_a_basics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 要求本练习库使用至少 C++17 的语言特性。
# PUBLIC 会把这一要求传递给链接该库的 demo 和 tests，CMake 会根据具体编译器选择
# 合适的标准选项。根工程已设置 CMAKE_CXX_EXTENSIONS=OFF，因此 GNU 编译器使用
# 标准 C++ 模式（通常为 -std=c++17），不会启用 gnu++17 扩展模式。
# 这里使用 target_compile_features，而不修改全局 CMAKE_CXX_STANDARD，避免练习影响其他 target。
target_compile_features(week1_tue_task_a_basics PUBLIC cxx_std_17)

# GNU 和 Clang 支持下面三项常用警告选项：
# - -Wall：打开一组常见且实用的警告；
# - -Wextra：补充更多潜在问题检查；
# - -Wpedantic：检查不符合标准 C++ 的编译器扩展。
#
# PRIVATE 表示这些警告选项只用于编译练习库，不会强制传递给链接它的其他 target。
# MSVC 的警告选项格式不同，因此不进入此分支，也不会收到无法识别的 GNU 风格参数。
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(week1_tue_task_a_basics PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 创建可直接运行的练习演示程序。
# 源文件 apps/demo.cpp 包含 main()，构建后通常生成：
#
#   build/cpp/exercises/Week1/Tue/task_A/week1_tue_task_a_demo
add_executable(week1_tue_task_a_demo
    apps/demo.cpp
)

# demo 会调用基础练习库中的接口，因此将 week1_tue_task_a_basics 链接进来。
# PRIVATE 表示这个依赖关系只属于 demo 本身；可执行程序通常不会再被其他 target 链接。
# 该链接关系也会让 CMake 在构建 demo 前自动先构建练习库。
target_link_libraries(week1_tue_task_a_demo PRIVATE week1_tue_task_a_basics)

# BUILD_TESTING 由根 CMakeLists.txt 中的 include(CTest) 创建，默认通常为 ON。
# 当用户配置 -DBUILD_TESTING=OFF 时，本练习仍会构建库和 demo，但不会创建或注册测试。
if(BUILD_TESTING)
    # 创建练习测试可执行目标。tests/cpp_basics_test.cpp 包含测试入口 main()；
    # 此步骤只定义如何编译测试程序，并不会在配置或普通构建时自动执行测试。
    add_executable(week1_tue_task_a_tests
        tests/cpp_basics_test.cpp
    )

    # 测试程序需要调用被测接口，因此链接基础练习库。
    # C++17 要求和公共头文件目录会通过该链接关系从练习库自动传递到测试 target。
    target_link_libraries(week1_tue_task_a_tests PRIVATE week1_tue_task_a_basics)

    # 向父工程已经启用的 CTest 注册一个测试：
    # - NAME 是 ctest 输出和 -R 正则筛选使用的稳定名称；
    # - COMMAND 使用上面定义的 target，CMake 会在生成阶段解析成测试程序的实际路径。
    #
    # add_test() 只登记测试。完成构建后，需在 aerial-control-lab 根目录执行：
    #
    #   ctest --test-dir build -R exercises.week1.tue.task_a --output-on-failure
    #
    # CTest 根据程序退出码判断结果：返回 0 表示通过，非 0 表示失败。
    add_test(
        NAME exercises.week1.tue.task_a
        COMMAND week1_tue_task_a_tests
    )
endif()
