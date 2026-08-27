# Week3 周一任务 D：智能指针与所有权

## 目标

- 使用 `unique_ptr` 管理多态对象；
- 使用 `make_shared` 建立共享所有权；
- 观察 `shared_ptr` 强引用计数变化；
- 使用 `weak_ptr::expired()` 判断观察对象是否过期；
- 使用 `weak_ptr::lock()` 安全取得临时强所有权；
- 理解 `weak_ptr` 为什么能够打破循环引用。

## `unique_ptr` 与多态

```cpp
std::unique_ptr<Resource> resource =
    std::make_unique<TextResource>();
```

- `make_unique<TextResource>()` 创建 `TextResource`，返回 `unique_ptr<TextResource>`；
- 所有权移动并转换给 `unique_ptr<Resource>`，实际对象仍是 `TextResource`；
- `Resource` 接口决定可以调用 `describe()`；
- 虚函数分派根据真实对象类型执行 `TextResource::describe()`；
- 基类虚析构保证通过 `Resource*` 删除对象时正确执行派生类和基类析构；
- `resource` 离开作用域后自动销毁对象，不使用裸 `new/delete`。

## `shared_ptr` 共享所有权

实际强计数变化：

```text
创建 first：1
复制出 second：2
second 离开作用域：1
first 离开作用域：0，对象销毁
```

复制的是 `shared_ptr` 所有权，不是复制被管理对象。通过 `second` 修改字符串后，`first` 能观察到同一对象的新值。

`use_count()` 仅用于本次学习和诊断，不用于正式业务中的生命周期或线程同步决策。

## `weak_ptr` 安全观察

实际运行结果：

```text
strong count after weak observer: 1
expired while owner exists: false
value through lock: observed resource
strong count during lock: 2
strong count after lock scope: 1
expired after owner is destroyed: true
lock succeeds after expiration: false
```

结论：

- `weak_ptr` 不拥有对象，不增加强引用计数；
- 对象存活时，`lock()` 返回临时 `shared_ptr` 并暂时增加强计数；
- 临时 `shared_ptr` 销毁后强计数恢复；
- 最后一个强所有者销毁后，对象析构，`weak_ptr` 变为过期状态；
- 对象过期后，`lock()` 返回空 `shared_ptr`；
- 实际访问时应直接检查 `lock()` 的结果，不能仅检查一次 `expired()` 后便假设对象一直存活。

## 循环引用

如果 A 和 B 相互使用 `shared_ptr`，外部所有者消失后，它们仍各自被对方强拥有，强计数无法降到零。把不承担所有权的一端改为 `weak_ptr` 后，该方向不再增加强计数，对象便能在外部强所有者消失后正常析构。

## 最小 API 表

| 类型 | API | 作用 |
|---|---|---|
| `unique_ptr` | `make_unique<T>()` | 创建对象并取得唯一所有权 |
| `unique_ptr` | `std::move(pointer)` | 转移唯一所有权 |
| `unique_ptr` | `if (pointer)` | 判断是否拥有对象 |
| `shared_ptr` | `make_shared<T>()` | 创建共享对象，初始强计数为 1 |
| `shared_ptr` | 复制智能指针 | 增加一个强所有者 |
| `shared_ptr` | `use_count()` | 查看当前强所有者数量，仅用于诊断 |
| `shared_ptr` | `reset()` | 当前指针放弃所有权，强计数减 1 |
| `weak_ptr` | 从 `shared_ptr` 赋值 | 建立不增加强计数的观察关系 |
| `weak_ptr` | `expired()` | 判断强计数是否已经为零 |
| `weak_ptr` | `lock()` | 存活时返回临时 `shared_ptr`，否则返回空指针 |

## 构建与运行

```powershell
cmake --preset windows-mingw-gcc-debug

cmake --build build/windows-mingw-gcc-debug `
    --target week3_mon_task_d_smart_pointers `
    -j 4

& '.\build\windows-mingw-gcc-debug\cpp\exercises\Week3\Mon\task_D\week3_mon_task_d_smart_pointers.exe'
```

## I/H/G 记录

- 当前等级：H（Hint）。
- 用户完成：多态对象创建与调用、`make_shared` 共享计数实验、`weak_ptr` 存活阶段的 `lock()` 实验。
- Codex 提供的帮助：多态与所有权解释、API 提示、过期阶段 `lock()` 补全、代码整理和文档总结。
- 验收结论：`make_unique`、`unique_ptr` 移动与多态、`make_shared`、`weak_ptr::lock()`、`expired()` 均形成可运行证据，达到周一 I/H 门槛。
