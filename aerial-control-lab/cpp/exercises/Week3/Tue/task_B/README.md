# Task B：五个 STL 最小编码任务

## 符号约定

- `n`：输入元素总数；
- `k`：请求返回的最大误差数量；
- `u`：不同离散状态的数量；
- `c`：滚动缓存的最大容量。

## 任务汇总

| 子任务 | 容器选择理由 | 时间复杂度 | 空间复杂度 | 边界行为 | 测试证据 |
|---|---|---|---|---|---|
| 固定三轴状态 | `std::array<double, 3>` 将“始终正好有三个分量”写入类型，元素连续存放，并支持标准容器接口 | `O(1)`；只初始化固定的三个元素 | `O(1)`；结果始终只有三个 `double` | 不存在空输入或零容量；接口直接接收 `x`、`y`、`z` 三个标量 | `ThreeAxisStateTest.MakeThreeAxisState` |
| 动态样本平均值 | `std::vector<double>` 支持运行时通过 `push_back()` 连续追加样本，并保持连续内存 | 记录 `n` 个样本的总成本为均摊 `O(n)`；`mean()` 使用 `accumulate` 遍历一次，为 `O(n)` | 样本容器自身为 `O(n)`；`mean()` 除输入和返回值外的额外空间为 `O(1)` | 空样本没有定义平均值，抛出 `std::invalid_argument` | `VectorMeanTest.ComputesMeanForDynamicallyAddedSamples`；`VectorMeanTest.RejectsEmptySamples` |
| 固定容量滚动缓存 | `std::deque<double>` 支持尾部加入最新样本和头部删除最旧样本，两端操作均为 `O(1)` | 在调用前满足 `size <= c` 的正常滚动场景中为 `O(1)`；若传入任意严重超限的旧缓存，单次最坏为 `O(n)` | 函数额外空间为 `O(1)`；缓存自身最多保存 `c` 个元素，为 `O(c)` | 容量为 `0` 时，处理后缓存始终为空 | `RollingBufferTest.KeepsNewestSamplesWithinCapacity`；`RollingBufferTest.LeavesBufferEmptyWhenCapacityIsZero` |
| 离散状态计数 | `std::unordered_map<std::string, std::size_t>` 适合不要求键有序的频次统计；`operator[]` 可在键缺失时插入零值再自增 | 平均 `O(n)`；哈希冲突极端严重时理论最坏为 `O(n^2)` | 结果保存 `u` 个不同状态，为 `O(u)`；最坏 `u = n` | 空状态列表合法，返回空 `unordered_map` | `StateCountsTest.CountsRepeatedStates`；`StateCountsTest.ReturnsEmptyCountsForEmptyStates` |
| 最大若干误差 | `std::priority_queue<double>` 持续维护当前最大元素；反复读取 `top()` 并 `pop()` 可按降序获得前 `k` 大误差 | 逐个压入 `n` 个误差为 `O(n log n)`，弹出 `k` 个为 `O(k log n)`；因 `k <= n`，整体简化为 `O(n log n)` | 堆保存 `n` 个元素，结果保存 `k` 个元素：`O(n + k)`；因 `k <= n`，简化为 `O(n)` | `k == 0` 返回空 `vector`；`k > n` 抛出 `std::invalid_argument` | `PriorityQueueTest.ReturnsLargestErrorsInDescendingOrder`；`PriorityQueueTest.ReturnsEmptyWhenCountIsZero`；`PriorityQueueTest.RejectsCountGreaterThanErrorCount` |

## 实现要点

- `mean()` 使用 `std::accumulate(samples.begin(), samples.end(), 0.0)`；初值 `0.0` 保证以 `double` 累加。
- 滚动缓存先 `push_back()`，再在 `size() > max_capacity` 时反复 `pop_front()`，从而自然覆盖零容量和已有缓存超限的情况。
- `count_states()` 使用 `++counts[state]`；键首次出现时，`operator[]` 先插入值初始化后的 `0`。
- `largest_errors()` 在读取 `top()` 后调用 `pop()`；`priority_queue::pop()` 本身不返回被删除元素。

## I/H/G 记录

- 五项功能实现、测试和 CMake 接入：H；
- 本 README 汇总表：G（由 Codex 按用户明确要求直接整理并写入）；
- 当前 task B 测试：10/10 通过；
- 写入本表时当前全量 CTest：45/45 通过。
