# 双平台构建与快速排错

本文用于定位同一份 `aerial-control-lab` 源码在 Ubuntu 与 Windows 上表现不同的问题。项目追求源码和测试跨平台通过，不追求二进制产物跨平台兼容。

## 1. 支持矩阵与验收优先级

| 优先级 | 操作系统 | 编译器 | 用途 |
| --- | --- | --- | --- |
| 权威 | Ubuntu 24.04.4 LTS | GCC/G++ 13.3.0 | 最终构建、测试，以及后续 PX4/ROS2/Gazebo 集成 |
| 次级 | Windows 11 专业版 | MinGW GCC/G++ 15.2.0（UCRT） | C++ 核心数学、Eigen、GoogleTest 和 Python 跨平台验证 |

MSVC 不在支持矩阵中。根 CMake 检测到 MSVC 时会直接停止，避免生成未经验证的结果。

## 2. 当前周四任务的生成来源

8 月 6 日任务 A、B、C 的源码和 CMake 配置是在以下环境生成并首次验证：

- Windows 11 专业版，版本 `10.0.26200`，64 位；
- MinGW GCC/G++ `15.2.0`，目标运行库为 UCRT；
- GNU Make `4.4.1`，命令名为 `mingw32-make`；
- Python `3.12.4`；
- NumPy `1.26.4`、pytest `7.4.4`；
- CMake `3.28.3` 便携版；系统 PATH 原本未安装 CMake，便携版只用于本次验证。

因此，如果 Ubuntu 首次构建失败，不要先假定数学实现错误。先保存 CMake 配置阶段输出，其中会明确打印主机系统、目标系统、编译器路径/版本和 CMake 版本，再按下文顺序检查。

Windows 最终验证结果为：完整 CTest/GoogleTest `5/5`、关闭 GoogleTest 的 CTest 兜底 `3/3`、pytest `40/40`，最终编译无警告。当前尚未在 Ubuntu 电脑上运行本次新增的 Eigen/GoogleTest 配置，因此 Linux 首次运行结果必须单独记录。

## 3. Linux 构建失败时的检查顺序

### 3.1 确认没有复用 Windows 构建目录

必须在 Ubuntu 仓库中创建新的构建树：

```bash
rm -rf build/ubuntu-gcc-debug
cmake --preset ubuntu-gcc-debug
```

不得复制或提交 Windows 的 `CMakeCache.txt`、`.o`、`.a`、`.dll`、`.exe` 或整个 `build/`。Windows MinGW 与 Ubuntu GCC 使用不同目标格式和运行库，这些文件没有跨平台兼容性。

### 3.2 核对实际工具链

```bash
g++ --version
cmake --version
cmake --preset ubuntu-gcc-debug
```

预期主环境是 GCC/G++ 13.3.0 和 CMake 3.28.3。若配置日志显示其他编译器路径，删除对应构建目录后重新配置；不要在已有 CMake cache 上直接更换编译器。

### 3.3 区分编译器新旧造成的诊断差异

本次 Windows 首次验证使用 GCC 15.2，而 Ubuntu 使用 GCC 13.3。两者都按标准 C++17、无 GNU 扩展构建，但警告集合和诊断文字可能不同：

- 仅 GCC 15.2 报警：检查是否是新版编译器新增诊断；
- 仅 GCC 13.3 编译失败：检查代码是否无意使用了 GCC 15 才支持的库接口；
- 两边结果不同：优先检查未定义行为、未初始化值、整数符号转换和浮点精确比较。

项目自己的 target 开启 `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion`，不使用 `-Werror`，因此警告应逐条处理，但不同版本新增警告不会直接阻断全部构建。

Eigen 的固定第三方 include 目录被标记为 CMake `SYSTEM`，避免 Eigen 内部警告淹没项目诊断。这不关闭项目源码警告；如果错误路径位于 `cpp/` 下，仍应正常处理。

### 3.4 检查固定依赖是否成功获取

项目固定使用：

- Eigen 3.4.1，提交 `d71c30c47858effcbd39967097a2d99ee48db464`；
- GoogleTest 1.17.0，提交 `52eb8108c5bdec04579160ae17225d66034bd723`。

首次配置需要访问 GitLab 和 GitHub。若网络失败，报错通常发生在 `_deps` 下载阶段，而不是项目源码编译阶段。

GoogleTest 暂时不可用时，可保留 CTest 兜底：

```bash
cmake -S . -B build/ubuntu-ctest-fallback \
  -DCMAKE_CXX_COMPILER=g++ \
  -DAERIAL_ENABLE_GOOGLETEST=OFF
cmake --build build/ubuntu-ctest-fallback
ctest --test-dir build/ubuntu-ctest-fallback --output-on-failure
```

若完全离线，可使用 `-DAERIAL_FETCH_DEPENDENCIES=OFF`，但需要预先安装完全匹配的 CMake package；否则 CMake 会明确报告缺少 Eigen3 或 GTest。

### 3.5 检查路径、大小写和换行

- Linux 文件名区分大小写，`#include` 的大小写必须与真实路径一致；
- CMake 和源码不得硬编码 `D:\\...` 或 Linux 用户目录；
- 仓库使用 `.gitattributes` 将 C++、Python、Markdown、JSON 和 CMake 文本规范化为 LF；
- 若 Git 显示整文件变化，先检查换行转换，不要直接覆盖另一台机器的有效修改。

### 3.6 检查浮点测试

Eigen 和 NumPy 的浮点结果允许平台相关的末位差异。测试应使用 `EXPECT_NEAR`、`isApprox` 或 `numpy.testing.assert_allclose`。若误差超过容差，记录输入、期望、实际值、编译器版本和是否启用优化，再判断是数值容差还是算法错误。

## 4. 建议保存的故障信息

在 Linux 报告问题时，至少保留：

```text
git rev-parse HEAD
git status --short
g++ --version
cmake --version
cmake --preset ubuntu-gcc-debug 的完整输出
cmake --build --preset ubuntu-gcc-debug 的首个错误
ctest --preset ubuntu-gcc-debug 的失败测试输出
PYTHONPATH=python python -m pytest python/tests -v 的输出
```

只报告最后一行“构建失败”通常不足以区分编译器、依赖、路径和源码问题；优先提供第一个错误及其前后上下文。
