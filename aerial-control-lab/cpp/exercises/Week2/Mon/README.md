# 8月17日周一练习

## 练习目标
这次的练习主要是摆脱使用codex进行一键生成的习惯，打算自己弄清楚所有的编译过程。
## 目录结构
include存放公共头文件，src存放cpp，app存放main.cpp，tests存放测试文件。
## 计划实现的功能
本练习计划实现以下两个小功能：

- 标量限幅：将输入值限制在给定的下界和上界之间；
- 三维向量有限性检查：判断向量中是否包含 NaN 或无穷大。

## 文件职责

- `include/control_utils.hpp`：声明供演示程序和测试程序调用的公共接口；
- `src/control_utils.cpp`：实现公共头文件中声明的函数；
- `app/control_utils_demo.cpp`：提供演示程序入口，调用并展示库中的功能；
- `tests/control_utils_test.cpp`：使用 GoogleTest 自动验证函数在正常输入、边界输入和异常输入下的行为；
- `CMakeLists.txt`：定义库、演示程序和测试程序的构建及链接关系；
- `README.md`：说明练习目标、目录结构、功能和各文件的职责。

## CMake targets

| Target | 类型 | 组成文件 | 链接依赖 | 最终产物或作用 |
|---|---|---|---|---|
| `week2_mon_control_utils` | 库 | `src/control_utils.cpp` | 公开依赖 `Eigen3::Eigen`，并公开 `include/` 头文件搜索路径 | 生成控制工具库；未启用 `BUILD_SHARED_LIBS` 时为静态库 |
| `week2_mon_control_utils_demo` | 可执行程序 | `app/control_utils_demo.cpp` | `week2_mon_control_utils` | 生成演示程序，用于调用并展示控制工具函数 |
| `week2_mon_control_utils_test` | 测试可执行程序 | `tests/control_utils_test.cpp` | `week2_mon_control_utils`、`GTest::gtest_main` | 生成 GoogleTest 测试程序，并由 `gtest_discover_tests` 将其中的测试用例注册给 CTest |

`week2_mon_control_utils` 的公共头文件使用了 `Eigen::Vector3d`，因此通过 `PUBLIC` 链接 `Eigen3::Eigen`。演示程序和测试程序链接该库后，会继承 Eigen 的使用要求。

`GTest::gtest_main` 提供 GoogleTest 框架和默认的 `main()` 入口。CTest 负责发现和运行已注册的测试，不是测试程序的链接依赖。
