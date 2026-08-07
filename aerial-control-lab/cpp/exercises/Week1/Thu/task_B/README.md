# 8 月 6 日（周四）任务 B：Python 四元数实现与测试

正式实现位于 [`python/aerial_control/quaternion.py`](../../../../../python/aerial_control/quaternion.py)，使用标量在前的 $[w,x,y,z]$ 顺序和 Hamilton 乘法，实现：

- `quat_normalize`；
- `quat_conjugate`；
- `quat_multiply`；
- `quat_rotate_vector`。

pytest 测试位于 [`python/tests/test_quaternion.py`](../../../../../python/tests/test_quaternion.py)，覆盖：

1. 单位四元数不改变向量；
2. 旋转后向量模长不变；
3. 旋转和逆旋转恢复原向量；
4. 两次旋转与 Hamilton 乘法组合一致；
5. 非单位四元数归一化后范数为 1；
6. $\mathbf{q}$ 与 $-\mathbf{q}$ 的旋转结果一致。

另外测试 NED 向下轴正转 $90^\circ$、共轭乘积、零范数、形状错误和非有限输入。

在 Linux 项目根目录执行：

```bash
PYTHONPATH=python python -m pytest python/tests/test_quaternion.py -v
```

在 Windows PowerShell 项目根目录执行：

```powershell
$env:PYTHONPATH = "python"
python -m pytest python/tests/test_quaternion.py -v
```
