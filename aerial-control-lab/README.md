# aerial-control-lab

用于学习和验证飞行器动力学、坐标变换与控制算法的最小 C++/Python 工程。

当前骨架只依赖 C++17 编译器、CMake 和 CTest；暂不引入 Eigen、GoogleTest 或飞控仿真依赖。

## 构建与测试

在仓库根目录依次执行：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

构建完成后可运行最小应用：

```bash
./build/sanity_check
```

运行第一周周二任务 A 的练习程序和单项测试：

```bash
./build/cpp/exercises/Week1/Tue/task_A/week1_tue_task_a_demo
ctest --test-dir build -R exercises.week1.tue.task_a --output-on-failure
```

每周练习默认参与构建。如只需正式核心库和应用，可以配置：

```bash
cmake -S . -B build -DAERIAL_BUILD_EXERCISES=OFF
```

需要验证干净构建时，删除 `build/` 后重新执行上述三条命令。

## 目录结构

```text
cpp/include/core/  C++ 公共头文件
cpp/src/           aerial_core 实现
cpp/apps/          可执行程序
cpp/tests/         CTest 测试程序
cpp/exercises/     按 WeekN/Day/task_X 分层的每日练习
python/            Python 包与测试
configs/           配置文件
docs/              项目文档
```

当前练习目录为：

```text
cpp/exercises/
└── Week1/
    └── Tue/
        └── task_A/
```

后续周次、日期和任务遵循相同命名规则，详见
[`cpp/exercises/README.md`](cpp/exercises/README.md)。
