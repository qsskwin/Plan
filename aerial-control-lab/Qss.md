当前项目采用“核心库、应用程序、测试、Python 验证、文档与配置分离”的结构：

```text
aerial-control-lab/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── cpp/
│   ├── include/core/
│   │   └── project_info.hpp
│   ├── src/
│   │   └── project_info.cpp
│   ├── apps/
│   │   └── sanity_check.cpp
│   └── tests/
│       └── aerial_core_test.cpp
├── python/
│   ├── aerial_control/
│   │   └── __init__.py
│   └── tests/
│       └── README.md
├── configs/
│   └── README.md
├── docs/
│   └── environment.md
├── build/
└── .git/
```

## 根目录

### `CMakeLists.txt`

[CMakeLists.txt](/home/laptop/Qss/Plan/aerial-control-lab/CMakeLists.txt) 是 C++ 项目的构建说明书，告诉 CMake：

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

[README.md](/home/laptop/Qss/Plan/aerial-control-lab/README.md) 是给项目使用者看的入口文档，包括：

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
project_info.hpp
```

头文件主要声明“有什么接口可以使用”，例如：

```cpp
std::string_view project_name() noexcept;
```

其他模块通过下面的方式包含它：

```cpp
#include "core/project_info.hpp"
```

以后这里可能出现：

```text
coordinate_transform.hpp
quaternion.hpp
rigid_body_state.hpp
rotation_matrix.hpp
```

`core` 表示这些接口属于项目核心库，不是 CMake 的特殊关键字，只是项目的目录命名。

### `cpp/src/`：核心库实现

当前文件：

```text
project_info.cpp
```

它负责实现头文件声明的函数：

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
sanity_check.cpp
```

这里放可以直接运行的程序，通常每个程序都有一个 `main()`：

```cpp
int main() {
    // 程序入口
}
```

当前构建结果是：

```text
build/sanity_check
```

后面可以增加不同的实验程序，例如：

```text
rotation_demo.cpp
quaternion_demo.cpp
simulation_runner.cpp
```

`apps/` 与 `src/` 的区别是：

- `src/` 负责实现可复用的库；
- `apps/` 负责组合这些库，形成可以启动的程序。

### `cpp/tests/`：C++ 测试

当前文件：

```text
aerial_core_test.cpp
```

这里放验证 C++ 核心库是否正确的自动化测试。当前测试检查 `project_name()` 是否返回预期结果。

后面可能包含：

```text
rotation_matrix_test.cpp
quaternion_test.cpp
coordinate_transform_test.cpp
```

测试代码不会成为正式应用的一部分，它只负责发现错误、防止旧功能被改坏。

当前关系是：

```text
                   aerial_core
                   核心静态库
                  ↙          ↘
        sanity_check       aerial_core_test
          正常应用            测试程序
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

当前只有 `__init__.py`，用于把目录标记为 Python 包。后面可以添加：

```text
rotations.py
quaternions.py
coordinate_frames.py
```

这个项目中 Python 主要适合：

- 快速验证数学公式；
- 使用 NumPy 对照 C++ 计算结果；
- 绘制仿真曲线；
- 在写 C++ 前验证算法思路。

### `python/tests/`

用于放 Python 测试，例如：

```text
test_rotations.py
test_quaternions.py
```

目前只有占位说明，还没有接入 pytest。

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

当前包含 [environment.md](/home/laptop/Qss/Plan/aerial-control-lab/docs/environment.md)，记录操作系统和工具版本。

后续还会放：

```text
coordinate_conventions.md
cpp_gap_list.md
dynamics_model.md
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
aerial_core_test
CMakeCache.txt
CMakeFiles/
```

这里的内容不应手动编辑，也不会提交到 Git。遇到构建缓存问题时，可以删除整个 `build/` 再重新生成：

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

删除 `build/` 不会丢失源码。

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
