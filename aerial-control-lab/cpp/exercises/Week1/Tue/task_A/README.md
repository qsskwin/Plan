# 8 月 4 日（周二）任务 A：C++ 基础练习

本目录是 `Week1/Tue/task_A`，包含 8 个围绕值、引用、指针、STL 与所有权的练习。7 个安全示例会被编译和测试；悬空引用错误示例只作为文档保存，不进入正式代码。

## 练习与对象生命周期

1. **引用交换**：`swap_by_reference` 借用调用者拥有的两个 `int`，对象至少要活到调用结束。参数是非常量引用，所以函数会修改两个输入。
2. **`const std::vector<double>&` 求和/均值**：`sum` 和 `mean` 只在调用期间借用调用者的容器，不复制也不拥有它。`const` 保证函数不能修改输入；空容器的平均值用 `std::nullopt` 表示。
3. **值传递与引用传递的复制对比**：`copies_seen_by_value` 得到一个属于函数的副本，而 `copies_seen_by_const_reference` 只借用原对象。两者都不修改调用者的对象，但只有值传递产生复制。
4. **指针判空**：`increment_if_not_null` 不拥有指针指向的整数，只在调用期间借用它。函数先检查空指针，非空时才修改输入，并用返回值报告是否成功。
5. **`std::array<double, 3>` 三维数据**：`Vec3` 在对象内部直接拥有三个定长元素，生命周期与数组变量一致。`scale_vec3` 只读借用输入并返回一个独立的新数组，不修改原值。
6. **`std::unordered_map` 字符串计数**：`count_strings` 只读借用字符串列表，并返回独立拥有键和值的哈希表。输入在函数内不会被修改，返回结果可在输入销毁后继续使用。
7. **悬空引用错误示例**：[`docs/dangling_reference_bad_example.md`](docs/dangling_reference_bad_example.md) 中的局部变量只活到函数结束，返回它的引用后引用立即悬空。错误代码不属于正式库，也不会参与编译。
8. **`std::unique_ptr` 最小所有权示例**：`make_owned_int` 返回对象的唯一所有权；调用者通过 `std::move` 把所有权交给 `consume_owned_int`。接管函数可以读取对象，函数结束时对象被自动释放，调用者的原指针变为空。

接口旁也保留了精简的所有权、生命周期和可修改性注释，见 [`include/task_a/cpp_basics.hpp`](include/task_a/cpp_basics.hpp)。

## 目录结构

```text
task_A/
├── CMakeLists.txt
├── README.md
├── apps/demo.cpp
├── docs/dangling_reference_bad_example.md
├── include/task_a/cpp_basics.hpp
├── src/cpp_basics.cpp
└── tests/cpp_basics_test.cpp
```

## 构建、运行与测试

在 `aerial-control-lab` 项目根目录执行：

```bash
cmake -S . -B build
cmake --build build
./build/cpp/exercises/Week1/Tue/task_A/week1_tue_task_a_demo
ctest --test-dir build -R exercises.week1.tue.task_a --output-on-failure
```

`week1_tue_task_a_demo` 展示各接口的运行结果，`week1_tue_task_a_tests` 覆盖正常输入、空容器、空指针、输入不被修改和所有权转移等行为。
