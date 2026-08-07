# Python 测试

在 `aerial-control-lab` 根目录执行：

```bash
PYTHONPATH=python python -m pytest python/tests -v
```

当前测试覆盖：

- $x$、$y$、$z$ 三个坐标轴的基础旋转矩阵；
- 旋转矩阵的正交性、行列式和向量模长不变性；
- 标量在前、Hamilton 约定下的四元数归一化、共轭、乘法和向量旋转；
- 四元数旋转的组合、逆变换、范数保持与 $\mathbf{q}/-\mathbf{q}$ 等价性；
- 非法形状、非有限数和零范数四元数的错误处理。

Windows PowerShell 使用：

```powershell
$env:PYTHONPATH = "python"
python -m pytest python/tests -v
```
