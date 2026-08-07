# aerial-control-lab

用于学习和验证飞行器动力学、坐标变换与控制算法的最小 C++/Python 工程。

C++ 工程使用标准 C++17、Eigen 3.4.1、GoogleTest 1.17.0 和 CTest；Python 旋转矩阵与四元数验证使用 NumPy 和 pytest。第三方 C++ 依赖由 CMake 固定到明确提交，避免 Ubuntu 与 Windows 获取不同版本。

支持的工具链只有：

- Ubuntu GCC/G++（权威主环境）；
- Windows MinGW GCC/G++（次级验证环境）。

MSVC 不受支持，CMake 检测到 MSVC 时会直接提示改用 MinGW GCC。双机角色和问题定位见 [`docs/build_troubleshooting.md`](docs/build_troubleshooting.md)。

## 第一周旋转模块

当前 `aerial_core` 已包含基于 Eigen 的最小四元数旋转接口：

- `normalizeQuaternion`：验证并归一化非零有限四元数；
- `composeRotation`：按“先第一个、再第二个”的明确顺序组合旋转；
- `rotateBodyToNed`：把 FRD 机体系向量转换到 NED 世界系；
- `rotateNedToBody`：执行上述变换的逆变换。

公共接口位于 [`cpp/include/core/rotation.hpp`](cpp/include/core/rotation.hpp)，约定见 [`docs/coordinate_conventions.md`](docs/coordinate_conventions.md)，Python/C++ 共用的固定用例见 [`docs/rotation_test_vectors.md`](docs/rotation_test_vectors.md)。

## 构建与测试

推荐使用与主机匹配的 CMake preset。

Ubuntu：

```bash
cmake --preset ubuntu-gcc-debug
cmake --build --preset ubuntu-gcc-debug
ctest --preset ubuntu-gcc-debug
```

Windows PowerShell（MinGW GCC，不使用 MSVC）：

```powershell
cmake --preset windows-mingw-gcc-debug
cmake --build --preset windows-mingw-gcc-debug
ctest --preset windows-mingw-gcc-debug
```

在 Linux 或已经显式选定 GNU generator 的环境中，也可以使用不带 preset 的通用命令；Windows 应优先使用上面的 MinGW preset，避免 CMake 自动选择 MSVC：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

构建完成后可运行最小应用：

```bash
./build/sanity_check
```

使用 preset 时，程序位于对应的 `build/<preset-name>/` 目录；Windows 可执行文件带 `.exe` 后缀。

运行第一周周二任务 A 的练习程序和单项测试：

```bash
./build/cpp/exercises/Week1/Tue/task_A/week1_tue_task_a_demo
ctest --test-dir build -R exercises.week1.tue.task_a --output-on-failure
```

运行第一周周三任务 A 的生命周期与 RAII 练习：

```bash
./build/cpp/exercises/Week1/Wed/task_A/week1_wed_task_a_demo
ctest --test-dir build -R exercises.week1.wed.task_a --output-on-failure
```

运行 Python 旋转矩阵测试：

```bash
PYTHONPATH=python python -m pytest python/tests -v
```

Windows PowerShell：

```powershell
$env:PYTHONPATH = "python"
python -m pytest python/tests -v
```

只运行 C++ 旋转 GoogleTest（应先完成对应 preset 的配置和构建）：

```bash
ctest --preset ubuntu-gcc-debug -R RotationUtilitiesTest
```

Windows PowerShell：

```powershell
ctest --preset windows-mingw-gcc-debug -R RotationUtilitiesTest
```

本次验证使用的 NumPy/pytest 版本记录在 [`requirements-dev.txt`](requirements-dev.txt)。已有可用环境不必为第一周主动重装；需要精确复现时再在各自机器的独立虚拟环境中安装该文件。

每周练习默认参与构建。如只需正式核心库和应用，可以配置：

```bash
cmake -S . -B build -DAERIAL_BUILD_EXERCISES=OFF
```

需要验证干净构建时，删除 `build/` 后重新执行上述三条命令。

## 目录结构

```text
cpp/include/core/  C++ 公共头文件
cpp/src/           aerial_core 实现
cpp/apps/          可执行程序
cpp/tests/         CTest 测试程序
cpp/exercises/     按 WeekN/Day/task_X 分层的每日练习
python/            Python 包与测试
configs/           配置文件
docs/              项目文档
```

当前练习目录为：

```text
cpp/exercises/
└── Week1/
    ├── Tue/
    │   ├── task_A/
    │   └── task_C/
    ├── Wed/
    │   ├── task_A/
    │   ├── task_B/
    │   └── task_C/
    ├── Thu/
    │   ├── task_A/
    │   ├── task_B/
    │   └── task_C/
    └── Fri/
        ├── task_A/
        ├── task_B/
        ├── task_C/
        └── task_E/
```

后续周次、日期和任务遵循相同命名规则，详见
[`cpp/exercises/README.md`](cpp/exercises/README.md)。

## 第三方依赖与离线兜底

首次 CMake 配置会下载固定提交的 Eigen 和 GoogleTest。若 GoogleTest 网络获取失败，可保留 CTest smoke test：

```bash
cmake -S . -B build/ctest-fallback -DAERIAL_ENABLE_GOOGLETEST=OFF
cmake --build build/ctest-fallback
ctest --test-dir build/ctest-fallback --output-on-failure
```

完全离线时可以使用 `-DAERIAL_FETCH_DEPENDENCIES=OFF`，但本机必须提供完全匹配版本的 Eigen3 和 GoogleTest CMake package。不要把另一台机器的 `_deps/` 或整个构建目录提交到 Git。
