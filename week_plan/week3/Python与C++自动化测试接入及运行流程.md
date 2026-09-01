# Python 与 C++ 自动化测试接入及运行流程

## 1. 文档目的与本周来源边界

这份文档记录如何在 `aerial-control-lab` 中添加 Python/pytest 测试，以及如何在 C++ 中使用 GoogleTest、CMake 和 CTest 完成同类验证。

第三周任务 D 和任务 E 都围绕 Euler/RK4 的接口、输入校验和自动化测试展开，验证意图高度相似。用户认为没有必要再次闭卷写出全部重复实现，因此主动要求 Codex 直接完成任务 D 的剩余 Python 实现与任务 E 的 C++ 实现。相关完整核心代码均按 G 记录，不作为独立重写证据。本阶段真正需要保留的工程能力是：知道应提前考虑哪些边界、测试文件放在哪里、测试如何被发现、如何接入构建系统，以及如何定向和全量运行测试。

## 2. 自动化测试的共同结构

无论 Python 还是 C++，一项测试通常包含三部分：

1. 准备输入和预期结果；
2. 调用被测函数；
3. 使用断言比较实际结果与预期结果，或确认预期异常确实发生。

本项目把业务实现和测试分开：

- 正式实现放在 `python/aerial_control` 或 `cpp/include`、`cpp/src`；
- 测试放在 `python/tests` 或 `cpp/tests`；
- Python 由 pytest 发现和运行测试；
- C++ 先由 CMake 编译测试程序，再由 GoogleTest 执行断言、CTest 统一发现和运行测试。

## 3. Python：添加 pytest 测试

### 3.1 需要创建或修改的文件

以积分器为例：

```text
aerial-control-lab/
└── python/
    ├── aerial_control/
    │   └── integrators.py
    └── tests/
        └── test_integrators.py
```

`integrators.py` 中放正式接口：

```python
def euler_step(derivative, t, state, dt):
    ...


def rk4_step(derivative, t, state, dt):
    ...
```

`test_integrators.py` 中导入待测函数并编写测试：

```python
import numpy as np
import pytest

from aerial_control.integrators import euler_step, rk4_step
```

### 3.2 pytest 如何发现测试

本项目采用 pytest 默认发现规则：

- 测试文件名使用 `test_*.py`，例如 `test_integrators.py`；
- 测试函数名以 `test_` 开头，例如 `test_euler_step_updates_state`；
- 测试函数内部使用普通 `assert`、`np.testing` 或 pytest 提供的断言辅助功能。

需要注意：只写普通 `assert` 或 `np.testing.assert_allclose` 时，不一定需要 `import pytest`。使用以下 pytest 功能时才需要显式导入：

- `pytest.raises(...)`：验证异常；
- `@pytest.mark.parametrize(...)`：参数化重复情境；
- fixture、marker 等 pytest API。

### 3.3 编写一个正常结果测试

测试应显式给出输入、调用和预期值：

```python
def test_euler_step_updates_state():
    state = np.array([1.0, 0.0])

    def derivative(t, current_state):
        del t
        return np.array([current_state[1], -current_state[0]])

    result = euler_step(derivative, 0.0, state, 0.1)

    np.testing.assert_allclose(result, [1.0, -0.1])
```

这里的测试调用了通用积分器，并把简谐振子动力学作为回调传入；积分器自身不应包含简谐振子专用逻辑。

### 3.4 验证异常与参数化边界

单一异常情境可以使用 `pytest.raises`：

```python
def test_euler_step_rejects_empty_state():
    derivative = lambda t, state: np.zeros_like(state)

    with pytest.raises(ValueError):
        euler_step(derivative, 0.0, np.array([]), 0.1)
```

多个同类输入可以使用参数化，避免复制整段测试：

```python
@pytest.mark.parametrize("invalid_dt", [0.0, -0.1, np.inf, np.nan])
def test_euler_step_rejects_invalid_dt(invalid_dt):
    state = np.array([1.0, 0.0])
    derivative = lambda t, current: np.zeros_like(current)

    with pytest.raises(ValueError):
        euler_step(derivative, 0.0, state, invalid_dt)
```

积分器至少应提前考虑：

- `dt` 是否为正且有限；
- 状态是否为非空一维 NumPy 数组；
- 状态是否全部有限；
- 导数返回值是否与状态同形状；
- 导数是否全部有限；
- 调用后原始状态是否保持不变。

### 3.5 从项目根目录运行 Python 测试

在 PowerShell 中进入项目根目录：

```powershell
cd D:\Qss\Qss\HNU\Private\Plan\aerial-control-lab
$env:PYTHONPATH = (Resolve-Path .\python).Path
```

固定解释器并运行单个测试文件：

```powershell
& D:\anaconda\python.exe -m pytest .\python\tests\test_integrators.py -q
```

只运行名称匹配的测试：

```powershell
& D:\anaconda\python.exe -m pytest .\python\tests\test_integrators.py -q -k rk4
```

查看 pytest 将收集哪些测试但不执行：

```powershell
& D:\anaconda\python.exe -m pytest .\python\tests\test_integrators.py --collect-only -q
```

运行整个 Python 测试集：

```powershell
& D:\anaconda\python.exe -m pytest .\python -q
```

本次实际结果为：任务 D 原有定向测试 `33/33`；任务 F 又补充 5 个 pytest case，当前积分器测试 `38/38`，Python 全量测试 `99/99`。

## 4. C++：添加 GoogleTest 并接入 CMake/CTest

### 4.1 需要创建或修改的文件

正式积分器使用以下结构：

```text
aerial-control-lab/
├── CMakeLists.txt
└── cpp/
    ├── include/core/integrators.hpp
    ├── src/integrators.cpp
    └── tests/integrators_test.cpp
```

各文件职责如下：

- `integrators.hpp`：声明公共类型和函数接口；
- `integrators.cpp`：实现 Euler、RK4 和输入/导数校验；
- `integrators_test.cpp`：使用 GoogleTest 调用接口并验证结果；
- 根 `CMakeLists.txt`：把源文件编进正式库，把测试文件编进测试程序，并注册给 CTest。

### 4.2 声明正式接口

头文件至少需要 Eigen 和 `std::function`：

```cpp
#pragma once

#include <Eigen/Core>
#include <functional>

namespace aerial_control {

using StateVector = Eigen::VectorXd;
using DerivativeFunction =
    std::function<StateVector(double, const StateVector&)>;

StateVector eulerStep(
    const DerivativeFunction& derivative,
    double t,
    const StateVector& state,
    double dt);

StateVector rk4Step(
    const DerivativeFunction& derivative,
    double t,
    const StateVector& state,
    double dt);

}  // namespace aerial_control
```

实现文件包含自己的头文件，在调用导数前检查 `dt` 和状态，在每次导数返回后检查维度和有限性。RK4 必须检查四次导数，而不是只检查 `k1`。

### 4.3 编写 GoogleTest

测试源文件需要引入 GoogleTest 和被测头文件：

```cpp
#include <gtest/gtest.h>

#include "core/integrators.hpp"
```

GoogleTest 使用 `TEST(测试套件名, 测试情境名)` 定义测试：

```cpp
TEST(CoreIntegratorTest, EulerIntegratesConstantDerivative) {
    aerial_control::StateVector state(2);
    state << 1.0, 3.0;

    const auto derivative = [](
        double t, const aerial_control::StateVector& current) {
        static_cast<void>(t);
        static_cast<void>(current);
        aerial_control::StateVector value(2);
        value << 2.0, -1.0;
        return value;
    };

    const auto result = aerial_control::eulerStep(
        derivative, 0.0, state, 0.1);

    EXPECT_NEAR(result[0], 1.2, 1e-12);
    EXPECT_NEAR(result[1], 2.9, 1e-12);
}
```

常用断言包括：

- `EXPECT_EQ`：整数、枚举或可精确比较的值；
- `EXPECT_NEAR(actual, expected, tolerance)`：浮点值；
- `EXPECT_TRUE(vector.isApprox(expected, tolerance))`：Eigen 向量；
- `EXPECT_THROW(expression, std::invalid_argument)`：异常路径。

测试函数不需要自己编写 `main()`，因为 CMake 链接 `GTest::gtest_main` 后会提供默认入口。

### 4.4 在 CMakeLists.txt 中编译正式实现

将实现源文件加入正式核心库：

```cmake
add_library(aerial_core
    cpp/src/integrators.cpp
    cpp/src/project_info.cpp
    cpp/src/rotation.cpp
)

target_include_directories(aerial_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/include>
)

target_link_libraries(aerial_core PUBLIC Eigen3::Eigen)
target_compile_features(aerial_core PUBLIC cxx_std_17)
```

头文件不需要列入 `add_library` 才能被包含；真正需要编译的 `.cpp` 必须列入 target。`PUBLIC` include 路径让测试 target 在链接 `aerial_core` 后能够写 `#include "core/integrators.hpp"`。

### 4.5 在 CMakeLists.txt 中接入 GoogleTest 与 CTest

本项目已经创建 `aerial_core_gtest`，因此只需把新测试源文件加入该 target：

```cmake
add_executable(aerial_core_gtest
    cpp/tests/eigen_vector_test.cpp
    cpp/tests/integrators_test.cpp
    cpp/tests/rotation_test.cpp
)

target_link_libraries(aerial_core_gtest
    PRIVATE
        aerial_core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(aerial_core_gtest DISCOVERY_TIMEOUT 30)
```

这里有三个不同阶段：

1. `add_executable`：把测试源码编译为测试程序；
2. `target_link_libraries`：链接正式核心库和 GoogleTest 默认入口；
3. `gtest_discover_tests`：构建后读取 GoogleTest 测试列表，并把每个 `TEST(...)` 注册为独立 CTest 测试。

因此，仅写 `TEST(...)` 但没有加入 CMake，测试不会被编译；仅编译测试程序但没有发现/注册，CTest 也无法按单个测试情境列出它们。

### 4.6 配置、编译和运行 C++ 测试

所有命令均从项目根目录运行：

```powershell
cd D:\Qss\Qss\HNU\Private\Plan\aerial-control-lab
```

第一次接入新源文件或修改 CMake 后重新配置：

```powershell
cmake --preset windows-mingw-gcc-debug
```

只编译正式 GoogleTest target，适合快速迭代：

```powershell
cmake --build --preset windows-mingw-gcc-debug --target aerial_core_gtest
```

定向运行任务 E 的积分器测试：

```powershell
ctest --test-dir build/windows-mingw-gcc-debug `
    -R '^CoreIntegrator' --output-on-failure
```

查看 CTest 已注册的测试但不执行：

```powershell
ctest --test-dir build/windows-mingw-gcc-debug -N
```

构建全部 target：

```powershell
cmake --build --preset windows-mingw-gcc-debug
```

运行全量 CTest：

```powershell
ctest --preset windows-mingw-gcc-debug
```

本次实际结果为：任务 E 新增测试 `17/17`，C++ 全量测试由 `67/67` 增至 `84/84`。MinGW GCC 15.2.0 编译新增源码时未产生新警告；配置阶段仍有 Eigen 上游已有的 CMake 策略弃用提示。

## 5. Python 与 C++ 测试流程对照

| 环节 | Python/pytest | C++/GoogleTest + CMake + CTest |
|---|---|---|
| 正式实现 | `python/aerial_control/*.py` | `cpp/include/*.hpp` + `cpp/src/*.cpp` |
| 测试文件 | `python/tests/test_*.py` | `cpp/tests/*_test.cpp` |
| 测试定义 | `def test_*():` | `TEST(Suite, Case) { ... }` |
| 异常检查 | `pytest.raises` | `EXPECT_THROW` |
| 浮点检查 | `np.testing.assert_allclose` | `EXPECT_NEAR` / Eigen `isApprox` |
| 接入步骤 | 设置 `PYTHONPATH` 后 pytest 自动发现 | CMake target 加源码、链接 GTest、注册给 CTest |
| 定向执行 | `pytest 文件 -k 关键词` | `ctest -R 正则` |
| 全量执行 | `python -m pytest python -q` | `ctest --preset windows-mingw-gcc-debug` |

## 6. 常见问题排查顺序

### 6.1 Python 测试未被发现

依次确认：

1. 文件名是否以 `test_` 开头；
2. 函数名是否以 `test_` 开头；
3. 是否从项目根目录设置了 `PYTHONPATH`；
4. 是否使用了计划指定的 `D:\anaconda\python.exe`；
5. 用 `--collect-only` 查看 pytest 实际收集结果。

### 6.2 C++ 测试未被发现

依次确认：

1. 测试文件是否加入 `add_executable`；
2. target 是否链接 `aerial_core` 与 `GTest::gtest_main`；
3. 是否调用 `gtest_discover_tests`；
4. 修改 CMake 后是否重新运行配置与构建；
5. 用 `ctest -N` 查看 CTest 注册结果。

### 6.3 测试失败时如何缩小范围

- Python：先运行单个文件，再用 `-k` 缩小到单个名称；
- C++：先只构建 `aerial_core_gtest`，再用 `ctest -R` 缩小到指定 suite；
- 保留 `--output-on-failure`，让 CTest 显示 GoogleTest 的失败断言；
- 先区分“编译失败”“测试未发现”和“断言失败”，三者处理路径不同。

## 7. 本阶段应记住的最小结论

1. pytest 主要靠 `test_*.py` 和 `test_*` 自动发现；`import pytest` 用于异常、参数化、fixture 等 pytest API。
2. GoogleTest 需要 `<gtest/gtest.h>` 和 `TEST(...)`；测试源码还必须加入 CMake target。
3. `GTest::gtest_main` 提供默认 `main()`，`gtest_discover_tests` 把测试注册给 CTest。
4. Python 直接解释运行；C++ 必须先配置和编译，再运行 CTest。
5. 正常结果、非法输入、维度、有限性、不修改输入和数值误差都应在设计接口时提前考虑，不必等出现故障后再补。
6. 本文记录的是工程接入与验证流程；任务 D/E 的完整积分器核心由用户主动要求 Codex 生成，评级保持 G。

任务 F 的跨语言测试验收现已完成：C++ 侧 17 个新增测试覆盖全部十类意图，Python 侧在任务 D 的 33 个 case 基础上直接补充常量导数、RK4/Euler 同步长误差对比和步长减半误差下降，共新增 5 个 case。任务 F 的补充代码同样由用户主动要求 Codex 直接完成，来源记为 G；当前 Python 全量 `99/99`、C++ 全量 `84/84`。
