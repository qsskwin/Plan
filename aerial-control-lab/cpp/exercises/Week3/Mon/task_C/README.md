# Week3 周一任务 C：`SampleBuffer`、RAII 与 GoogleTest

## 设计决定

- 默认构造得到合法的空缓冲区。
- 内部使用 `std::vector<double>` 管理样本，不使用裸 `new/delete`。
- `at()` 使用 `vector::at()`，越界时抛出 `std::out_of_range`。
- 允许默认拷贝，拷贝后的两个缓冲区拥有相互独立的数据。
- 允许默认移动；目标对象取得样本，源对象保持有效但内容未指定。
- 遵循 Rule of Zero，不手写析构、拷贝或移动函数。

## GoogleTest 与 CTest 的分工

- GoogleTest 用于编写测试用例和断言，并生成测试可执行程序。
- CTest 用于运行已注册的测试、筛选测试并汇总通过/失败结果。
- `gtest_discover_tests()` 会发现测试程序中的每个 `TEST`，并将其分别注册到 CTest。

## 将 GoogleTest 接入 CMake

library target 先编译 `SampleBuffer`：

```cmake
add_library(
    week3_mon_task_c_sample_buffer
    src/sample_buffer.cpp
)

target_include_directories(
    week3_mon_task_c_sample_buffer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

测试 target 编译测试源码，同时链接被测库和 GoogleTest 提供的默认 `main()`：

```cmake
if(TARGET GTest::gtest_main)
    add_executable(
        week3_mon_task_c_sample_buffer_test
        tests/sample_buffer_test.cpp
    )

    target_link_libraries(
        week3_mon_task_c_sample_buffer_test
        PRIVATE
            week3_mon_task_c_sample_buffer
            GTest::gtest_main
    )

    aerial_enable_warnings(
        week3_mon_task_c_sample_buffer_test
    )

    include(GoogleTest)

    gtest_discover_tests(
        week3_mon_task_c_sample_buffer_test
        TEST_PREFIX "exercises.week3.mon.task_c."
        DISCOVERY_TIMEOUT 30
    )
endif()
```

关键关系：

```text
sample_buffer.cpp
        ↓ add_library
静态库 .a
        ↓ target_link_libraries
测试源码 + GTest::gtest_main
        ↓ add_executable
GoogleTest 测试程序 .exe
        ↓ gtest_discover_tests
CTest 中的独立测试项
```

## 配置、编译与运行

在 `aerial-control-lab` 根目录执行：

```powershell
cmake --preset windows-mingw-gcc-debug

cmake --build build/windows-mingw-gcc-debug `
    --target week3_mon_task_c_sample_buffer_test `
    -j 4

ctest --test-dir build/windows-mingw-gcc-debug `
    -R "exercises.week3.mon.task_c" `
    --output-on-failure
```

## 常用 GoogleTest API

| API | 用途 |
|---|---|
| `TEST(Suite, Name)` | 定义测试；第一个参数是测试套件名，第二个参数是测试名 |
| `EXPECT_TRUE(value)` | 期望布尔表达式为真 |
| `EXPECT_FALSE(value)` | 期望布尔表达式为假 |
| `EXPECT_EQ(actual, expected)` | 期望两个普通值相等 |
| `EXPECT_DOUBLE_EQ(actual, expected)` | 比较两个 `double`，考虑浮点表示误差 |
| `EXPECT_NEAR(actual, expected, tolerance)` | 在指定绝对误差范围内比较浮点数 |
| `EXPECT_THROW(statement, type)` | 期望语句抛出指定类型的异常 |
| `EXPECT_NO_THROW(statement)` | 期望语句不抛异常 |
| `ASSERT_*` | 失败时结束当前测试函数；其他测试仍可继续 |

`EXPECT_*` 失败后会继续执行当前测试。只有后续检查依赖某个前置条件时，才优先使用 `ASSERT_*`。

## 覆盖范围

1. 默认构造为空；
2. 添加样本后更新 `empty()` 和 `size()`；
3. 按插入顺序读取样本；
4. 越界读取抛出 `std::out_of_range`；
5. 拷贝后数据相互独立；
6. 移动后目标取得数据，源对象可以重新赋值并继续使用。

所有测试中的局部 `SampleBuffer` 都在作用域结束时自动析构，不需要手动释放资源。

## I/H/G 记录

- 当前等级：H（Hint）。
- 用户独立完成：类接口、`std::vector` 委托实现、CMake library target、首个 GoogleTest 和基本断言。
- Codex 提供的帮助：接口与 CMake 提示、测试覆盖补齐、常用断言说明和文档整理。
- 验收结论：类实现和测试达到 I/H 门槛；无裸 `new/delete`，RAII 由 `std::vector` 和对象作用域保证。



### 7.1 GoogleTest/CTest 接入与编译

任务 C 使用一个被测 library target 和一个测试 executable target。测试程序同时链接被测库与 `GTest::gtest_main`：

```cmake
add_executable(
    week3_mon_task_c_sample_buffer_test
    tests/sample_buffer_test.cpp
)

target_link_libraries(
    week3_mon_task_c_sample_buffer_test
    PRIVATE
        week3_mon_task_c_sample_buffer
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(
    week3_mon_task_c_sample_buffer_test
    TEST_PREFIX "exercises.week3.mon.task_c."
    DISCOVERY_TIMEOUT 30
)
```

各部分作用：

- `add_executable`：把测试源码编译为 `.exe`；
- `target_link_libraries(... week3_mon_task_c_sample_buffer)`：让测试调用被测类的实现；
- `GTest::gtest_main`：链接 GoogleTest 框架及默认 `main()`，测试文件无需自行编写入口；
- `include(GoogleTest)`：加载 CMake 的 GoogleTest 集成命令；
- `gtest_discover_tests`：将每个 `TEST` 自动注册为独立 CTest 测试；
- 根 CMake 的固定 GoogleTest 依赖先提供 `GTest::gtest_main` target，任务 C 在 `if(TARGET GTest::gtest_main)` 内创建测试。

配置、编译和运行命令：

```powershell
cmake --preset windows-mingw-gcc-debug

cmake --build build/windows-mingw-gcc-debug `
    --target week3_mon_task_c_sample_buffer_test `
    -j 4

ctest --test-dir build/windows-mingw-gcc-debug `
    -R "exercises.week3.mon.task_c" `
    --output-on-failure
```

实际结果：任务 C 6/6 通过；当前构建树全量 CTest 35/35 通过。

### 7.2 常用 GoogleTest API

| API | 作用 | 当前用法 |
|---|---|---|
| `TEST(Suite, Name)` | 定义测试套件和测试用例 | 每个核心意图建立独立测试 |
| `EXPECT_TRUE(value)` | 期望条件为真 | 验证默认缓冲区为空 |
| `EXPECT_FALSE(value)` | 期望条件为假 | 验证添加样本后不为空 |
| `EXPECT_EQ(actual, expected)` | 比较普通值 | 验证 `size()` |
| `EXPECT_DOUBLE_EQ(actual, expected)` | 比较两个 `double` | 验证样本值和插入顺序 |
| `EXPECT_NEAR(actual, expected, tolerance)` | 按指定绝对误差比较浮点数 | 后续数值算法测试使用 |
| `EXPECT_THROW(statement, type)` | 验证指定异常 | 验证越界抛出 `std::out_of_range` |
| `EXPECT_NO_THROW(statement)` | 验证不抛异常 | 后续正常路径需要时使用 |
| `ASSERT_*` | 失败时停止当前测试函数 | 仅在后续检查依赖前置条件时使用 |

补充区别：标准库小写 `assert(...)` 不属于 GoogleTest，也不属于 CTest；它在定义 `NDEBUG` 后可能被禁用。本项目测试应优先使用 `EXPECT_*`/`ASSERT_*`。

任务详细记录：[Week3 周一任务 C](../../aerial-control-lab/cpp/exercises/Week3/Mon/task_C/README.md)
