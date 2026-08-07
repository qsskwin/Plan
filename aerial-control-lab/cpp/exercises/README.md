# 每周练习目录约定

这里存放学习计划产生的练习代码。目录固定按“周 → 星期 → 任务”三级组织，正式项目代码仍放在 `cpp/include/core`、`cpp/src`、`cpp/apps` 和 `cpp/tests` 中。

## 命名规则

- 周：`Week1`、`Week2`、`Week3`……
- 星期：`Mon`、`Tue`、`Wed`、`Thu`、`Fri`、`Sat`、`Sun`
- 任务：`task_A`、`task_B`、`task_C`……
- CMake target：使用小写的 `weekN_day_task_x_用途`，例如 `week1_tue_task_a_tests`
- CTest 名称：使用 `exercises.weekN.day.task_x`，例如 `exercises.week1.tue.task_a`

扩展后的结构示例：

```text
cpp/exercises/
├── Week1/
│   ├── Mon/
│   │   └── task_A/
│   └── Tue/
│       ├── task_A/
│       └── task_B/
├── Week2/
│   └── Mon/
│       └── task_A/
└── Week3/
    └── Wed/
        └── task_C/
```

只在真正开始某项任务时创建对应目录，不预建空的周、日期或任务文件夹。每个练习目录至少应有说明文档，并根据内容提供源码、测试或推导笔记。

## 当前内容

- [`Week1/Tue/task_A`](Week1/Tue/task_A/README.md)：值、引用、指针、STL 与所有权基础练习。
- [`Week1/Tue/task_C`](Week1/Tue/task_C/README.md)：坐标系约定文档的任务索引。
- [`Week1/Wed/task_A`](Week1/Wed/task_A/README.md)：对象生命周期、RAII、`ScopeTimer` 与独占所有权练习。
- [`Week1/Wed/task_B`](Week1/Wed/task_B/README.md)：基本旋转矩阵推导、合法性验证与乘法顺序说明。
- [`Week1/Wed/task_C`](Week1/Wed/task_C/README.md)：Python 旋转矩阵实现与测试的任务索引。
- [`Week1/Thu/task_A`](Week1/Thu/task_A/README.md)：四元数顺序、Hamilton 乘法、轴角、向量旋转与组合推导。
- [`Week1/Thu/task_B`](Week1/Thu/task_B/README.md)：Python 四元数实现与 6 类核心测试的任务索引。
- [`Week1/Thu/task_C`](Week1/Thu/task_C/README.md)：Eigen、GoogleTest、CTest 兜底与双平台 GCC 构建说明。
- [`Week1/Fri/task_A`](Week1/Fri/task_A/README.md)：正式 C++/Eigen 旋转接口、输入策略和 Eigen 系数顺序。
- [`Week1/Fri/task_B`](Week1/Fri/task_B/README.md)：GoogleTest 覆盖与 Python/C++ 共享测试向量。
- [`Week1/Fri/task_C`](Week1/Fri/task_C/README.md)：ZYX 欧拉角定义、奇异性和使用限制。
- [`Week1/Fri/task_E`](Week1/Fri/task_E/README.md)：干净构建、完整测试与警告检查记录。
