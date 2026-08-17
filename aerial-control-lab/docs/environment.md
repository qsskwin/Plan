# 开发环境

## 双机开发策略

本项目使用两台电脑开发，但角色不同：

- Ubuntu 是权威主环境，负责最终构建、完整测试，以及后续 PX4、ROS2、Gazebo 集成；
- Windows 是次级验证环境，只负责 C++ 核心数学、Eigen、GoogleTest、Python 和文档的跨平台检查；
- 两台机器只通过 Git 共享源码、测试和配置，不共享任何构建目录、虚拟环境或二进制产物；
- Windows 只使用 MinGW GCC/G++，明确不使用 MSVC。

若两个环境结果冲突，先在 Ubuntu 主环境复现并作为最终验收依据，同时按 [`build_troubleshooting.md`](build_troubleshooting.md) 保存两边工具链和首个错误。

记录日期：2026-08-01。

### Ubuntu 权威环境

| 项目 | 版本 |
| --- | --- |
| 操作系统 | Ubuntu 24.04.4 LTS (Noble Numbat) |
| C++ 编译器 | GCC/G++ 13.3.0 (`c++ (Ubuntu 13.3.0-6ubuntu2~24.04.1)`) |
| CMake | 3.28.3 |
| Git | 2.43.0 |
| Python | 3.12.3 |

### Windows 次级验证环境

记录日期：2026-08-17。

| 项目 | 版本 |
| --- | --- |
| 操作系统 | Windows 11 专业版，10.0.26200，64 位 |
| C++ 编译器 | MinGW GCC/G++ 15.2.0，UCRT |
| 构建工具 | GNU Make 4.4.1（`mingw32-make`） |
| CMake | 4.4.2（`C:\Program Files\CMake\bin\cmake.exe`） |
| Python | 3.12.4（Anaconda） |
| Python 测试依赖 | NumPy 1.26.4、pytest 7.4.4 |
| 明确不使用 | MSVC、Visual Studio CMake generator |

## 最小构建命令

在仓库根目录执行：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

运行最小应用：

```bash
./build/sanity_check
```

Eigen 和 GoogleTest 从周四开始由 CMake 获取固定版本。首次配置需要网络；依赖版本、离线兜底和跨平台故障定位见 [`build_troubleshooting.md`](build_troubleshooting.md)。

2026-08-17 复核时，实际使用的工具版本为 CMake 4.4.2、MinGW G++ 15.2.0 和 Python 3.12.4。Eigen 源码中的 CMake 策略警告来自上游依赖；当前项目可以正常配置和构建，因此不修改 `build/**/_deps`，也不重新安装 Eigen。

## 8 月 6 日 Windows 验证结果

- CMake 成功识别 `Windows + GNU 15.2.0 + MinGW Makefiles`，未使用 MSVC；
- Eigen 3.4.1 与 GoogleTest 1.17.0 固定依赖获取成功；
- 完整 C++ 构建通过，最终无编译警告；
- GoogleTest/CTest 最终通过 `5/5`；
- 关闭 GoogleTest 的 CTest 兜底通过 `3/3`；
- Python 3.12.4 下 pytest 通过 `40/40`。

当前机器无法代替 Ubuntu 电脑完成 Linux 实机验收。Ubuntu 首次执行时应使用 `ubuntu-gcc-debug` preset，并按 [`build_troubleshooting.md`](build_troubleshooting.md) 保存配置日志；如果失败，日志中的工具链来源可直接与本节 Windows 生成环境对照。
