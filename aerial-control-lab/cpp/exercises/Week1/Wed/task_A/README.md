# 8 月 5 日（周三）任务 A：类、对象生命周期与 RAII

本目录通过一个最小生命周期观察类、一个作用域计时器和一个独占所有权示例，练习构造、析构、拷贝、移动与 RAII。

## 生命周期观察

`LifecycleProbe` 借用调用者拥有的 `LifecycleStats`。每次直接构造、拷贝构造、移动构造和析构都会更新相应计数，因此可以在语句执行后和作用域结束后观察对象生命周期。

练习中的移动构造会把原值交给新对象，并把移动源的值设为 $0$。这是本类明确规定的可观察状态；对于一般标准库或第三方类型，只能假设移动后的对象仍然有效，其具体值可能未指定。

## `ScopeTimer`

`ScopeTimer` 构造时通过单调时钟记录开始时间，析构时计算持续时间

$$
\Delta t = t_{\mathrm{end}} - t_{\mathrm{start}}
$$

并以 $\mu\mathrm{s}$ 输出。计时器不拥有输出流，因此输出流必须比计时器活得更久。拷贝和移动均被禁用，避免同一个计时区间被多个对象重复结束。

## `new/delete` 与 RAII

手工管理动态对象时，每条控制路径都必须正确执行 `delete`：

```cpp
auto* probe = new LifecycleProbe(stats, 7);
// 中间若提前返回或抛出异常，delete 可能被跳过。
delete probe;
```

本练习的正式代码使用 `std::unique_ptr`：

```cpp
auto owner = make_owned_probe(stats, 7);
```

`owner` 离开作用域或执行 `reset()` 时会自动析构对象。所有权只能通过 `std::move` 显式转移，因此正常返回、提前返回和异常展开都不需要手工释放资源。正式代码应优先使用 RAII；裸指针只表达不拥有的临时观察关系时才考虑使用。

## 目录结构

```text
task_A/
├── CMakeLists.txt
├── README.md
├── apps/demo.cpp
├── include/week1_wed_task_a/lifecycle_raii.hpp
├── src/lifecycle_raii.cpp
└── tests/lifecycle_raii_test.cpp
```

## 构建、运行与测试

在 `aerial-control-lab` 根目录执行：

```bash
cmake -S . -B build
cmake --build build
./build/cpp/exercises/Week1/Wed/task_A/week1_wed_task_a_demo
ctest --test-dir build -R exercises.week1.wed.task_a --output-on-failure
```

仍需继续确认的 copy/move 问题记录在项目级 [`docs/cpp_gap_list.md`](../../../../../docs/cpp_gap_list.md)。
