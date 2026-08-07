# 8 月 6 日（周四）任务 C：Eigen 与 GoogleTest

本任务把 Eigen 作为 `aerial_core` 的公共依赖，并新增独立 GoogleTest target `aerial_core_gtest`。原有 `aerial_core_test` 继续作为不依赖 GoogleTest 的 CTest smoke test。

## 1. 固定依赖

为避免 Ubuntu 与 Windows 获取不同源码，根 `CMakeLists.txt` 固定：

- Eigen 3.4.1：提交 `d71c30c47858effcbd39967097a2d99ee48db464`；
- GoogleTest 1.17.0：提交 `52eb8108c5bdec04579160ae17225d66034bd723`。

默认使用 CMake `FetchContent` 下载。若网络受阻：

1. 可以先设置 `-DAERIAL_ENABLE_GOOGLETEST=OFF`，保留最小 CTest smoke test；
2. 可以设置 `-DAERIAL_FETCH_DEPENDENCIES=OFF`，但本机必须提供完全匹配的 Eigen3 和 GoogleTest CMake 包；
3. 详细定位步骤见 [`docs/build_troubleshooting.md`](../../../../../docs/build_troubleshooting.md)。

## 2. 支持的编译器

- Ubuntu：GCC/G++；
- Windows：MinGW GCC/G++；
- MSVC：明确不支持，CMake 检测到后直接停止并提示改用 MinGW GCC。

项目使用标准 C++17，关闭 GNU 语言扩展，并为本项目 target 开启：

```text
-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
```

警告不会直接整体关闭，也暂不全局提升为错误。

Eigen 是固定的第三方头文件依赖，其 include 目录被标记为 CMake `SYSTEM`。这样 GCC 15.2 不会用项目级 `-Wsign-conversion` 重复报告 Eigen 内部实现；项目自己的源码仍保留全部警告检查。

## 3. 测试 target

[`cpp/tests/eigen_vector_test.cpp`](../../../../tests/eigen_vector_test.cpp) 验证 Eigen 三维向量加法、点积和范数。

接入时先故意把向量加法的 $z$ 分量期望值从 `3.5` 写成 `4.0`，运行一次测试并确认 GoogleTest 能报告测试名、源码位置、实际值和期望值；观察失败报告后再将期望值修正为 `3.5`。最终提交状态只保留修正后的通过测试。

## 4. 构建命令

Ubuntu：

```bash
cmake --preset ubuntu-gcc-debug
cmake --build --preset ubuntu-gcc-debug
ctest --preset ubuntu-gcc-debug
```

Windows PowerShell：

```powershell
cmake --preset windows-mingw-gcc-debug
cmake --build --preset windows-mingw-gcc-debug
ctest --preset windows-mingw-gcc-debug
```

两个预设分别使用独立构建目录，任何 `build/` 内容都不得通过 Git 在两台机器之间共享。

## 5. 本次实际验证记录

验证日期：2026-08-06。

实际环境：Windows 11 专业版、MinGW GCC/G++ 15.2.0（UCRT）、GNU Make 4.4.1、CMake 3.28.3 便携版。未调用或配置 MSVC。

验证过程与结果：

1. CMake 配置成功，日志明确显示 `host=Windows`、`compiler=GNU 15.2.0`、`CMake=3.28.3`；
2. 故意错误的 Eigen 期望值产生预期 GoogleTest 失败，报告包含测试名、源码位置、实际值 `3.5` 和错误期望值 `4.0`；
3. 将期望值修正为 `3.5` 后重新编译，最终无编译警告；
4. 完整 CTest/GoogleTest 通过 `5/5`；
5. 设置 `AERIAL_ENABLE_GOOGLETEST=OFF` 的兜底构建通过 `3/3`。

Ubuntu GCC 13.3 实机验证需要在另一台电脑执行，不能由本次 Windows 结果替代。若 Linux 失败，优先参照 [`docs/build_troubleshooting.md`](../../../../../docs/build_troubleshooting.md) 中的生成来源和检查顺序。
