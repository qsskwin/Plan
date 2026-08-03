# 要求使用 CMake 3.16 或更高版本来配置本项目。
# 如果用户的 CMake 版本低于 3.16，配置阶段会直接报错，而不是使用可能不兼容的行为。
cmake_minimum_required(VERSION 3.16)

# 声明一个名为 aerial_control_lab 的 CMake 项目：
# - VERSION 0.1.0：设置项目版本，并产生 PROJECT_VERSION 等变量；
# - LANGUAGES CXX：本项目当前只启用 C++，因此 CMake 只检测 C++ 编译器。
#
# 注意：这里的项目名使用下划线，不必与磁盘目录 aerial-control-lab 完全相同。
project(aerial_control_lab VERSION 0.1.0 LANGUAGES CXX)

# 加载 CTest 支持。它主要完成两件事：
# 1. 创建 BUILD_TESTING 选项（默认值通常为 ON）；
# 2. 当 BUILD_TESTING=ON 时启用测试，并提供 test、Continuous、Experimental 等辅助目标。
#
# 用户可以在配置时通过 -DBUILD_TESTING=OFF 关闭测试目标的构建。
include(CTest)

# 每周练习与正式核心库分开构建。默认启用，若只需要 aerial_core，
# 可在配置时传入 -DAERIAL_BUILD_EXERCISES=OFF。
option(AERIAL_BUILD_EXERCISES "Build Week/Day/Task learning exercises" ON)

# 创建名为 aerial_core 的库目标，并指定组成该库的源文件。
#
# 这里没有显式写 STATIC 或 SHARED，因此库类型由 BUILD_SHARED_LIBS 决定；
# 在未设置 BUILD_SHARED_LIBS 的默认情况下，它会被构建为静态库 libaerial_core.a。
# 后续坐标变换、动力学和控制算法的公共实现可以继续加入这个目标。
add_library(aerial_core
    cpp/src/project_info.cpp
)

# 设置 aerial_core 的公共头文件搜索路径。
# 这样源文件就能写 #include "core/project_info.hpp"，而不需要使用很长的相对路径。
#
# PUBLIC 表示该头文件路径同时用于：
# - 编译 aerial_core 本身；
# - 编译所有链接 aerial_core 的目标，例如 sanity_check 和 aerial_core_test。
#
# BUILD_INTERFACE 和 INSTALL_INTERFACE 是“生成器表达式”，分别表示：
# - 在源码目录中直接构建时，头文件位于 cpp/include；
# - 将来安装该库后，使用安装位置下的 include 目录。
# 当前项目还没有 install() 规则，INSTALL_INTERFACE 是为以后安装/导出库预留的。
target_include_directories(aerial_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/include>
        $<INSTALL_INTERFACE:include>
)

# 要求 aerial_core 使用至少 C++17 的语言特性。
# 使用 PUBLIC 后，这项要求也会传递给链接 aerial_core 的其他目标，
# 因而 sanity_check 和 aerial_core_test 也会采用兼容 C++17 的编译选项。
# 这种写法比手动添加 -std=c++17 更容易兼容不同编译器。
target_compile_features(aerial_core PUBLIC cxx_std_17)

# 创建名为 sanity_check 的可执行目标。
# 它由 cpp/apps/sanity_check.cpp 编译，最终通常生成 build/sanity_check。
add_executable(sanity_check
    cpp/apps/sanity_check.cpp
)

# 将 aerial_core 链接到 sanity_check。
# PRIVATE 表示只有 sanity_check 自己需要这个直接链接关系，
# 不要求未来依赖 sanity_check 的其他目标继续传递该关系。
# 构建 sanity_check 时，CMake 会先自动构建它所依赖的 aerial_core。
target_link_libraries(sanity_check PRIVATE aerial_core)

# 练习目录按 WeekN/Day/task_X 分层。每个练习使用包含周、日、任务名的
# 唯一 target 名称，避免后续新增 Week2、Mon 或 task_B 时发生重名。
if(AERIAL_BUILD_EXERCISES)
    add_subdirectory(cpp/exercises/Week1/Tue/task_A)
endif()

# 只有在 BUILD_TESTING=ON 时才创建测试程序并注册 CTest 测试。
# include(CTest) 默认打开该选项，所以普通配置会进入此分支；
# 如需只构建库和应用，可使用 cmake -S . -B build -DBUILD_TESTING=OFF。
if(BUILD_TESTING)
    # 创建测试可执行目标，最终通常生成 build/aerial_core_test。
    # 这一步只是“编译测试程序”，还没有运行测试。
    add_executable(aerial_core_test
        cpp/tests/aerial_core_test.cpp
    )

    # 测试程序需要调用核心库中的接口，因此将它链接到 aerial_core。
    target_link_libraries(aerial_core_test PRIVATE aerial_core)

    # 向 CTest 注册一个测试：
    # - NAME aerial_core.smoke：测试在 CTest 中显示的名称；
    # - COMMAND aerial_core_test：运行测试时执行上面构建出的程序。
    #
    # add_test() 只负责注册。构建后还需要执行 ctest --test-dir build，
    # CTest 才会真正运行该程序，并依据退出码判断成功（0）或失败（非 0）。
    add_test(NAME aerial_core.smoke COMMAND aerial_core_test)
endif()
