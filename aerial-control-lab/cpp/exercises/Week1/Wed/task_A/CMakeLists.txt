# 本文件是 Week1/Wed/task_A 的子目录构建说明，不是独立 CMake 工程入口。
# 因此这里不重复调用 cmake_minimum_required() 和 project()；配置根工程时，根
# CMakeLists.txt 通过下面的命令读取本文件：
#
#   add_subdirectory(cpp/exercises/Week1/Wed/task_A)
#
# 进入本目录后：
# - CMAKE_CURRENT_SOURCE_DIR 指向当前 task_A 源码目录；
# - CMake 在总构建目录下建立 cpp/exercises/Week1/Wed/task_A 对应的构建子目录；
# - 本文件定义的 target 会加入根工程的统一构建图，可以按 target 名单独构建。

# 创建生命周期与 RAII 练习库，并把实现文件加入该 target。
# target 名称包含周、星期和任务编号，避免与其他日期的练习重名。
# 未显式写 STATIC 或 SHARED，因此库类型遵循父工程的 BUILD_SHARED_LIBS 设置；
# 默认未开启 BUILD_SHARED_LIBS 时通常生成静态库 libweek1_wed_task_a_lifecycle.a。
# 练习库与正式 aerial_core 相互独立，不会把教学用 LifecycleProbe 混入核心库。
add_library(week1_wed_task_a_lifecycle
    src/lifecycle_raii.cpp
)

# 设置练习库的公共头文件搜索目录，使源文件可以写：
#
#   #include "week1_wed_task_a/lifecycle_raii.hpp"
#
# CMAKE_CURRENT_SOURCE_DIR 在这里就是当前 task_A 目录，实际加入的路径为 task_A/include。
# PUBLIC 表示该路径用于两类编译：
# 1. 编译 week1_wed_task_a_lifecycle 自身；
# 2. 编译所有链接该库的下游 target，例如 demo 和 tests。
# 因此 demo/tests 不需要重复写 target_include_directories。
target_include_directories(week1_wed_task_a_lifecycle
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 声明该库及其下游使用者至少需要 C++17。
# 本练习使用 std::make_unique、移动语义、属性等现代 C++ 功能；PUBLIC 会把要求自动
# 传递给 demo 和 tests。根工程设置了 CMAKE_CXX_EXTENSIONS=OFF，因此 GNU 编译器
# 使用标准 C++17 模式，而不是额外启用 gnu++17 扩展。
target_compile_features(week1_wed_task_a_lifecycle PUBLIC cxx_std_17)

# 只在 GNU/Clang 编译器上添加兼容的警告选项：
# - -Wall：启用一组常见问题警告；
# - -Wextra：启用额外的参数、类型等检查；
# - -Wpedantic：提醒代码是否依赖非标准语言扩展。
#
# PRIVATE 表示这些选项只用于编译本练习库，不会传递给 demo、tests 或其他 target。
# MSVC 使用不同的警告参数，因此不进入这个条件分支。
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        week1_wed_task_a_lifecycle
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

# 创建可直接运行的演示程序。apps/demo.cpp 提供 main()，用于观察：
# - 普通构造、拷贝构造、移动构造和析构计数；
# - std::unique_ptr 的所有权转移；
# - ScopeTimer 离开作用域时自动输出耗时。
#
# 使用 Windows preset 时，输出通常位于：
# build/windows-mingw-gcc-debug/cpp/exercises/Week1/Wed/task_A/
# week1_wed_task_a_demo.exe
add_executable(week1_wed_task_a_demo
    apps/demo.cpp
)

# demo 调用了练习库中定义的 LifecycleProbe、ScopeTimer 和工厂函数，因此必须链接库。
# PRIVATE 表示链接关系只属于 demo；可执行程序不会再把该依赖传播给其他 target。
# 依赖图也保证构建 demo 时，CMake 会先构建 week1_wed_task_a_lifecycle。
target_link_libraries(
    week1_wed_task_a_demo
    PRIVATE
        week1_wed_task_a_lifecycle
)

# BUILD_TESTING 由根工程的 include(CTest) 创建，默认通常为 ON。
# 当用户配置 -DBUILD_TESTING=OFF 时，本练习仍保留库和 demo，但不会生成测试可执行程序，
# 也不会向 CTest 注册本练习测试。
if(BUILD_TESTING)
    # 创建测试程序。tests/lifecycle_raii_test.cpp 自带 main()，通过返回码向 CTest 报告结果。
    # add_executable 只定义如何编译该程序；普通构建会生成它，但不会自动运行它。
    add_executable(week1_wed_task_a_tests
        tests/lifecycle_raii_test.cpp
    )

    # 测试通过公开头文件调用被测库，使用方式与真实调用者一致。
    # PUBLIC include 路径和 C++17 要求会经由该链接关系自动传递给测试 target。
    target_link_libraries(
        week1_wed_task_a_tests
        PRIVATE
            week1_wed_task_a_lifecycle
    )

    # 向 CTest 注册一个稳定名称：
    # - NAME 用于测试报告，也可被 ctest -R 的正则表达式筛选；
    # - COMMAND 引用上面创建的 target，CMake 会解析成实际可执行文件路径。
    #
    # add_test 只登记测试，不会立即执行。完成构建后可以运行：
    #
    #   ctest --preset windows-mingw-gcc-debug \
    #     -R exercises.week1.wed.task_a --output-on-failure
    #
    # 测试进程返回 0 表示通过，返回非 0 表示失败。
    add_test(
        NAME exercises.week1.wed.task_a
        COMMAND week1_wed_task_a_tests
    )
endif()
