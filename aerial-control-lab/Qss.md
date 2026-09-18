当前项目采用“核心库、应用程序、测试、Python 验证、文档与配置分离”的结构：

```text
aerial-control-lab/
├── CMakeLists.txt
├── README.md
├── Qss.md
├── .gitignore
├── cpp/
│   ├── include/core/
│   │   ├── altitude_pd.hpp
│   │   ├── integrators.hpp
│   │   ├── point_mass.hpp
│   │   ├── project_info.hpp
│   │   └── rotation.hpp
│   ├── src/
│   │   ├── altitude_pd.cpp
│   │   ├── integrators.cpp
│   │   ├── point_mass.cpp
│   │   ├── project_info.cpp
│   │   └── rotation.cpp
│   ├── apps/
│   │   ├── altitude_pd_demo.cpp
│   │   └── sanity_check.cpp
│   └── tests/
│       ├── aerial_core_test.cpp
│       ├── altitude_pd_test.cpp
│       ├── eigen_vector_test.cpp
│       ├── integrators_test.cpp
│       ├── point_mass_test.cpp
│       └── rotation_test.cpp
├── python/
│   ├── aerial_control/
│   │   ├── integrators.py
│   │   ├── point_mass.py
│   │   ├── quaternion.py
│   │   ├── rotation.py
│   │   └── simulation.py
│   └── tests/
│       ├── test_integrators.py
│       ├── test_point_mass.py
│       ├── test_quaternion.py
│       └── test_rotation.py
├── configs/
│   └── README.md
├── docs/
│   ├── build_troubleshooting.md
│   ├── coordinate_conventions.md
│   ├── cpp_gap_list.md
│   ├── environment.md
│   └── rotation_test_vectors.md
├── build/
└── .git/
```

## 根目录

### `CMakeLists.txt`

[CMakeLists.txt](CMakeLists.txt) 是 C++ 项目的构建说明书，告诉 CMake：

- 项目叫什么；
- 使用什么 C++ 标准；
- 哪些文件组成核心库；
- 哪些文件生成可执行程序；
- 程序需要链接哪些库；
- 注册哪些测试。

可以理解为：

```text
C++ 源文件 + CMakeLists.txt
              ↓
            CMake
              ↓
       库、程序和测试程序
```

### `README.md`

[README.md](README.md) 是给项目使用者看的入口文档，包括：

- 项目用途；
- 如何配置；
- 如何编译；
- 如何测试；
- 如何运行程序。

### `.gitignore`

告诉 Git 哪些生成文件不应该纳入版本管理，例如：

```text
build/
*.o
*.a
__pycache__/
.venv/
```

源码需要提交，编译产生的文件通常不提交。

## `cpp/`：C++ 代码

这是项目的主要 C++ 实现目录。

### `cpp/include/core/`：公共头文件

当前文件：

```text
altitude_pd.hpp
integrators.hpp
point_mass.hpp
project_info.hpp
rotation.hpp
```

头文件主要声明“有什么接口可以使用”，例如：

```cpp
std::string_view project_name() noexcept;
```

其他模块通过下面的方式包含它：

```cpp
#include "core/project_info.hpp"
```

`core` 表示这些接口属于项目核心库，不是 CMake 的特殊关键字，只是项目的目录命名。

### `cpp/src/`：核心库实现

当前文件：

```text
altitude_pd.cpp
integrators.cpp
point_mass.cpp
project_info.cpp
rotation.cpp
```

这些文件负责实现头文件声明的函数。例如 `project_info.cpp` 实现：

```cpp
std::string_view project_name() noexcept {
    return "aerial-control-lab";
}
```

`include/` 和 `src/` 的基本分工是：

```text
include/：告诉别人怎么调用
src/：具体完成函数功能
```

这些实现会被 CMake 编译成：

```text
build/libaerial_core.a
```

这是一个静态库，不能直接运行，必须被应用程序或测试程序链接。

### `cpp/apps/`：可执行应用

当前文件：

```text
altitude_pd_demo.cpp
sanity_check.cpp
```

这里放可以直接运行的程序，通常每个程序都有一个 `main()`：

```cpp
int main() {
    // 程序入口
}
```

当前主要构建结果包括：

```text
build/sanity_check
build/altitude_pd_demo
```

`altitude_pd_demo` 复用核心库中的高度 PD、RK4 和质点动力学，按固定采样周期运行闭环。其内部职责已经分为：

```text
AltitudePdController::compute  计算控制
runCase                       推进工况并保存样本
summarizeCase                 汇总结果
exportSamplesCsv              导出已有样本
```

`apps/` 与 `src/` 的区别是：

- `src/` 负责实现可复用的库；
- `apps/` 负责组合这些库，形成可以启动的程序。

### `cpp/tests/`：C++ 测试

当前文件：

```text
aerial_core_test.cpp
altitude_pd_test.cpp
eigen_vector_test.cpp
integrators_test.cpp
point_mass_test.cpp
rotation_test.cpp
```

这里放验证 C++ 核心库行为的自动化测试，当前覆盖项目骨架、Eigen 向量、旋转、积分器、质点平动和高度 PD 控制器。

测试代码不会成为正式应用的一部分，它只负责发现错误、防止旧功能被改坏。

当前关系是：

```text
                         aerial_core
                         核心静态库
                 ↙            ↓             ↘
      sanity_check   altitude_pd_demo   aerial_core_gtest
         应用              闭环演示           测试程序
```

## `python/`：Python 验证代码

### `python/aerial_control/`

这是 Python 包目录。因为 Python 模块名不能使用连字符，所以使用：

```text
aerial_control
```

而不是：

```text
aerial-control
```

当前包除 `__init__.py` 外还包括：

```text
integrators.py
point_mass.py
quaternion.py
rotation.py
simulation.py
```

这个项目中 Python 主要适合：

- 快速验证数学公式；
- 使用 NumPy 对照 C++ 计算结果；
- 绘制仿真曲线；
- 在写 C++ 前验证算法思路。

### `python/tests/`

当前已有：

```text
test_integrators.py
test_point_mass.py
test_quaternion.py
test_rotation.py
```

这些测试已经接入 pytest。本文档更新没有重新运行 Python 全量测试；历史运行结果与当前是否回归通过应分别记录。

## `configs/`：配置文件

以后用于保存不适合写死在代码里的参数，例如：

```text
vehicle.yaml
controller.yaml
simulation.yaml
```

可能包含：

- 飞行器质量；
- 惯性矩阵；
- 重力加速度；
- 仿真步长；
- 控制器参数。

当前只有 README 占位文件。

## `docs/`：项目文档

当前包含：

```text
build_troubleshooting.md
coordinate_conventions.md
cpp_gap_list.md
environment.md
rotation_test_vectors.md
```

这里适合记录代码本身难以表达的内容，例如：

- NED/FRD 坐标约定；
- 四元数旋转方向；
- 单位约定；
- 数学推导；
- 设计决策。

## `build/`：编译产物

这是 CMake 自动生成的构建目录，包含：

```text
libaerial_core.a
sanity_check
altitude_pd_demo
aerial_core_test
CMakeCache.txt
CMakeFiles/
```

这里的内容不应手动编辑，也不会提交到 Git。日常应复用与工具链匹配的 preset 构建目录；需要验证干净构建时，创建一个路径明确的新构建目录，不删除已有构建证据。例如：

```powershell
cmake --preset windows-mingw-gcc-debug -B .\build\clean-check
cmake --build .\build\clean-check --parallel 4
```

生成文件不属于源码；不要将构建目录加入 Git。

## `.git/`：Git 仓库数据

这是 Git 自动维护的隐藏目录，保存：

- 提交历史；
- 分支；
- 标签；
- 暂存区；
- 仓库配置。

不要手动修改其中的文件，应通过 `git` 命令操作。

整体上，可以把项目理解为：

```text
docs/、python/         推导并验证思路
        ↓
cpp/include + cpp/src  实现可复用核心库
        ↓
cpp/apps               运行实际程序
cpp/tests              自动验证实现
        ↓
build/                  保存编译结果
```
