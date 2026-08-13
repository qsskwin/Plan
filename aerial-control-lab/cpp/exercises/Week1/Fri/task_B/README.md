# 8 月 7 日（周五）任务 B：GoogleTest 与跨语言核对

正式 GoogleTest 位于 [`cpp/tests/rotation_test.cpp`](../../../../tests/rotation_test.cpp)，并加入现有 `aerial_core_gtest` 构建目标。

共享输入和期望值统一记录在 [`docs/rotation_test_vectors.md`](../../../../../docs/rotation_test_vectors.md)。Python 与 C++ 均以该文档为用例来源。

## 覆盖范围

1. 单位旋转；
2. 绕 NED 向下轴正转 90°；
3. 先绕 x 轴、再绕 z 轴的组合顺序；
4. 机体系到 NED、再从 NED 返回机体系；
5. 非单位四元数归一化；
6. `q` 与 `-q` 的姿态等价性；
7. 零范数、非有限四元数和非有限向量的拒绝策略；
8. 与周四 Python 测试相同的固定输入和期望值。

## 运行方式

Windows MinGW GCC：

```powershell
cmake --preset windows-mingw-gcc-debug
cmake --build --preset windows-mingw-gcc-debug
ctest --preset windows-mingw-gcc-debug -R RotationUtilitiesTest
```

Ubuntu GCC：

```bash
cmake --preset ubuntu-gcc-debug
cmake --build --preset ubuntu-gcc-debug
ctest --preset ubuntu-gcc-debug -R RotationUtilitiesTest
```

完整 Python 核对：

```bash
PYTHONPATH=python python -m pytest python/tests -v
```
