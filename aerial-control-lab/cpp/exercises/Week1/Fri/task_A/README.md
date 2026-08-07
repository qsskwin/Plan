# 8 月 7 日（周五）任务 A：最小 C++/Eigen 旋转接口

本任务将四元数旋转作为正式 `aerial_core` 接口实现，而不是建立新的练习专用数学类。

## 1. 正式实现位置

- 公共接口：[`cpp/include/core/rotation.hpp`](../../../../include/core/rotation.hpp)
- 核心实现：[`cpp/src/rotation.cpp`](../../../../src/rotation.cpp)
- 构建 target：根 [`CMakeLists.txt`](../../../../../CMakeLists.txt) 中的 `aerial_core`

接口包括：

```cpp
normalizeQuaternion(quaternion)
composeRotation(first_rotation, second_rotation)
rotateBodyToNed(q_nb, vector_body)
rotateNedToBody(q_nb, vector_ned)
```

`composeRotation(first, second)` 明确表示先执行 `first`，再执行 `second`。在列向量和 Hamilton 约定下，实现顺序为 `second * first`。

## 2. 所有权和输入策略

- 输入均使用 `const&`，函数不取得对象所有权，也不修改输入；
- 返回新值，不暴露内部引用或裸指针；
- 有限的非单位四元数会在内部归一化；
- 零范数、NaN 或无穷大四元数抛出 `std::invalid_argument`；
- 含 NaN 或无穷大的向量同样抛出 `std::invalid_argument`。

这样可以接受数值积分后存在小幅范数漂移的姿态，同时避免零四元数被静默解释为有效旋转。

## 3. Eigen 系数顺序陷阱

`Eigen::Quaterniond` 的构造参数是：

```text
(w, x, y, z)
```

但 `quaternion.coeffs()` 返回：

```text
(x, y, z, w)
```

实现使用 `w()`、`x()`、`y()`、`z()` 命名访问器进行归一化复制；测试也通过命名访问器核对结果，禁止假设两种接口具有相同下标顺序。
