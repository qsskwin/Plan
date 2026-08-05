# Python 测试

在 `aerial-control-lab` 根目录执行：

```bash
PYTHONPATH=python python -m pytest python/tests -v
```

当前测试覆盖 $x$、$y$、$z$ 三个坐标轴的基础旋转矩阵，以及正交性、行列式和向量模长不变性。
