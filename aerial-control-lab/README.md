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

运行正式 C++ 积分器测试（Windows PowerShell；应先完成对应 preset 的配置）：

```powershell
cmake --build --preset windows-mingw-gcc-debug --target aerial_core_gtest
ctest --preset windows-mingw-gcc-debug -R "^CoreIntegrator" --output-on-failure
```

运行正式 Python 积分器定向测试和 Python 全量测试。本机已核对的解释器为
`D:\anaconda\python.exe`，`PYTHONPATH` 设置仅对当前 PowerShell 会话及其子进程生效：

```powershell
$env:PYTHONPATH = (Resolve-Path .\python).Path
& D:\anaconda\python.exe -m pytest .\python\tests\test_integrators.py -q
& D:\anaconda\python.exe -m pytest .\python -q
```

本次验证使用的 NumPy/pytest 版本记录在 [`requirements-dev.txt`](requirements-dev.txt)。已有可用环境不必为第一周主动重装；需要精确复现时再在各自机器的独立虚拟环境中安装该文件。

## 第四周三维质点平动与跨语言演示

`aerial_core` 和 Python 包均提供固定姿态下的三维质点平动模型。状态顺序为
`[p_n,p_e,p_d,v_n,v_e,v_d]`：前三项是 NED 位置（m），后三项是 NED
速度（m/s），其导数依次为速度和加速度。

- NED 世界系的 `+z` 指向 Down；FRD 机体系的 `+z` 也指向 Down。
- `q_NB/R_NB` 把机体系 B 中的向量转换到 NED 世界系 N。
- Python 四元数数组顺序为 `[w,x,y,z]`；`Eigen::Quaterniond` 构造参数顺序
  为 `(w,x,y,z)`，但 `coeffs()` 返回 `(x,y,z,w)`。
- `T` 是非负总推力标量，实际 FRD 推力向量为 `[0,0,-T]`；NED 重力为
  `[0,0,+g]`，其中 `g=9.80665 m/s²`。
- 模型只包含平动；质量、姿态和推力在每个工况中保持常量，不包含姿态积分、
  转动动力学、执行器或反馈控制器。
- Python 演示复用 `simulate_fixed_step` 和 `rk4_step`，C++ 演示复用
  `rk4Step`、`pointMassDerivative` 和 `rotateBodyToNed`。两端均使用双精度、
  整数步索引，并保存初始状态和终点。

Windows PowerShell 中运行三个统一工况：

```powershell
$env:PYTHONPATH = (Resolve-Path .\python).Path
& D:\anaconda\python.exe .\python\exercises\Week4\Fri\task_A\point_mass_demo.py

cmake --preset windows-mingw-gcc-debug
cmake --build .\build\windows-mingw-gcc-debug --target point_mass_demo -j 4
& .\build\windows-mingw-gcc-debug\cpp\exercises\Week4\Fri\task_A\point_mass_demo.exe

& D:\anaconda\python.exe .\python\exercises\Week4\Fri\task_B\compare_point_mass_outputs.py
```

两端使用相同的六维零初态和 `dt=0.01 s`。自由落体使用 `m=1.5 kg`、
`T=0`、单位姿态和 `tf=2 s`；悬停使用 `T=mg=14.709975 N`、单位姿态和
`tf=10 s`；固定正滚转使用 `roll=10°`、`T=mg/cos(roll)` 和 `tf=2 s`。
每个保存时刻均与独立解析解比较，并再次逐时刻、逐分量比较 Python/C++；
使用 `rtol=0`、位置 `atol=1e-10 m`、速度 `atol=1e-10 m/s`。

| 工况 | 样本数 | Python/C++→解析最大位置误差 (m) | Python/C++→解析最大速度误差 (m/s) | Python↔C++位置/速度误差 |
|---|---:|---:|---:|---:|
| 自由落体 | 201 | `2.1316282072803006e-14` | `3.907985046680551e-14` | `0 / 0` |
| 水平悬停 | 1001 | `0` | `0` | `0 / 0` |
| 固定正滚转 | 201 | `9.3258734068513149e-15` | `1.3322676295501878e-14` | `0 / 0` |

质量偏差实验保持名义悬停推力 `14.709975 N`，只把实际质量改为
`1.65 kg`，使用单位姿态、零初态、`dt=0.01 s` 和 `tf=2 s`。运行命令为：

```powershell
$env:PYTHONPATH = (Resolve-Path .\python).Path
& D:\anaconda\python.exe .\python\exercises\Week4\Fri\task_A\point_mass_demo.py --mass-bias-only
& .\build\windows-mingw-gcc-debug\cpp\exercises\Week4\Fri\task_A\point_mass_demo.exe --mass-bias-only
& D:\anaconda\python.exe .\python\exercises\Week4\Fri\task_C\compare_mass_bias_outputs.py
```

此时 Down 合力为 `1.4709975 N`，Down 加速度为
`0.8915136363636364 m/s²`；`t=2 s` 的解析位置为
`[0,0,1.783027272727273] m`，解析速度为
`[0,0,1.783027272727273] m/s`。Python/C++→解析的最大位置/速度误差分别为
`2.886579864025407e-15 m` 和 `4.6629367034256575e-15 m/s`，两端完整轨迹
互差为零。

原推力只是按名义质量计算的固定前馈平衡输入。质量变化后，它不会依据位置或
速度偏差自动修正。第五周反馈应使用竖直位置和速度；若高度定义为 `h=-p_d`，
则 `h_dot=-v_d`。也可以直接使用 Down 位置 `p_d` 和速度 `v_d`，但参考值、
误差符号和推力方向必须统一约定。本周不实现控制器。

上述命令只针对周五新增演示和比较程序，不代表当前 CTest、GoogleTest 或 pytest
全量回归；干净构建与全量验收按计划留到周六。

VS Code 中可用左侧 Source Control 视图检查同一批改动：`Changes` 表示尚未暂存，
文件右侧 `+` 会加入暂存区；`Staged Changes` 表示下一次提交的预览，文件右侧 `-`
可取消暂存。点击文件可查看工作区与基线的逐行 diff。检查时不要把 `build/`、
`__pycache__/` 或临时文件加入版本控制，也不要为了查看 diff 而实际提交。本次周五
文件可以在不 stage/unstage 的情况下直接预览，并使用 `git status --short`、
`git diff -- README.md CMakeLists.txt` 和未跟踪文件清单交叉核对。

每周练习默认参与构建。如只需正式核心库和应用，可以配置：

```bash
cmake -S . -B build -DAERIAL_BUILD_EXERCISES=OFF
```

需要验证干净构建时，不删除现有构建目录；应选择一个尚不存在、路径明确的新构建目录。例如在 Windows PowerShell 中运行：

```powershell
cmake --preset windows-mingw-gcc-debug -B .\build\week2-sat-clean-20260822
cmake --build .\build\week2-sat-clean-20260822 --parallel 4
ctest --test-dir .\build\week2-sat-clean-20260822 --output-on-failure
```

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
