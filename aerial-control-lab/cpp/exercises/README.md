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
