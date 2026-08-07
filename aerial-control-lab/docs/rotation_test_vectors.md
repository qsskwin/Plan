# Python/C++ 共享旋转测试向量

本文档是 Python 与 C++ 旋转实现共同使用的固定用例来源。两端不得为了让各自测试通过而独立更换输入或期望值。

## 1. 共同约定

- 四元数按标量在前的 `[w, x, y, z]` 书写；
- 四元数乘法使用 Hamilton 约定；
- 使用右手坐标系和列向量；
- `q_NB` 把 FRD 机体系向量转换到 NED 世界系；
- 浮点比较绝对误差使用 `1e-12`；
- Python 数组直接按 `[w, x, y, z]` 保存；
- `Eigen::Quaterniond` 构造参数为 `(w, x, y, z)`，但 `coeffs()` 返回 `(x, y, z, w)`，不得通过相同下标在两者之间复制。

## 2. 固定用例

### TV-01：单位旋转

```text
q_NB      = [1, 0, 0, 0]
v_B       = [1.5, -2.0, 4.25]
expected  = [1.5, -2.0, 4.25]
```

单位四元数不改变向量；反向变换结果也相同。

### TV-02：绕 NED 向下轴正转 90°

令 `s = sqrt(2) / 2`：

```text
q_NB      = [s, 0, 0, s]
v_B       = [1, 0, 0]
expected  = [0, 1, 0]
```

机体前向由 NED 北向旋转到东向。

### TV-03：组合旋转

先绕 x 轴正转 90°，再绕 z 轴正转 90°：

```text
q_x       = [sqrt(2)/2, sqrt(2)/2, 0, 0]
q_z       = [sqrt(2)/2, 0, 0, sqrt(2)/2]
q_total   = q_z ⊗ q_x = [0.5, 0.5, 0.5, 0.5]
v         = [0, 1, 0]
expected  = [0, 0, 1]
```

这同时固定 `composeRotation(first, second)` 的语义：先应用 `first`，再应用 `second`。

### TV-04：正向与逆向恢复

```text
axis      = normalize([0.3, 0.7, -0.2])
angle     = -0.83 rad
v_B       = [-1.0, 2.5, 0.75]
v_N       = rotateBodyToNed(q_NB, v_B)
recovered = rotateNedToBody(q_NB, v_N)
expected  = [-1.0, 2.5, 0.75]
```

同时要求 `norm(v_N) == norm(v_B)`。

### TV-05：非单位四元数归一化

```text
q         = [2, -2, 1, 3]
norm(q)   = sqrt(18)
expected  = [2, -2, 1, 3] / sqrt(18)
```

归一化后的范数必须为 1。

### TV-06：双覆盖符号等价

```text
axis      = [0, 1, 0]
angle     = 0.61 rad
v_B       = [1, 2, 3]
expected  = rotate(q, v_B) == rotate(-q, v_B)
```

比较旋转结果，不直接要求四元数四个系数逐项相等。

## 3. 无效输入

以下输入必须明确失败，而不是返回 NaN 或静默使用未定义姿态：

- `[0, 0, 0, 0]` 零范数四元数；
- 含 NaN 或无穷大的四元数；
- 含 NaN 或无穷大的三维向量；
- Python 中形状不是 `(4,)` 的四元数或形状不是 `(3,)` 的向量。

## 4. 对应测试

- Python：[`python/tests/test_quaternion.py`](../python/tests/test_quaternion.py)
- C++：[`cpp/tests/rotation_test.cpp`](../cpp/tests/rotation_test.cpp)

两端当前共同覆盖 TV-01 至 TV-06；语言自身的类型和形状检查可以另外增加，但不得改变上述共同语义。
