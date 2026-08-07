#pragma once  // 防止同一翻译单元重复包含本头文件，避免类型与函数被重复定义。

#include <chrono>  // std::chrono::steady_clock：为作用域计时器提供单调时钟。
#include <cstddef>  // std::size_t：表示各类生命周期事件的非负计数。
#include <iosfwd>   // 前置声明 std::ostream，头文件无需引入完整的 <ostream>。
#include <memory>   // std::unique_ptr：表达动态对象的唯一所有权。
#include <string>   // std::string：由 ScopeTimer 独立保存计时标签。

// Week1 / Wed / task_A：类、对象生命周期、移动语义与 RAII 练习接口。
// 独立命名空间可避免练习中的类型名称与项目其他模块发生冲突。
namespace week1_wed_task_a {

// 汇总 LifecycleProbe 发生过的各类生命周期事件。
//
// 这是一个聚合类型，四个计数器都用成员初始化器从 0 开始。多个 LifecycleProbe
// 可以引用同一个 LifecycleStats，从而把一组相关对象的事件集中记录在一起。
// LifecycleStats 始终由调用者拥有；它必须比所有引用它的 LifecycleProbe 活得更久，
// 否则 LifecycleProbe 在构造或析构时访问 stats_ 会形成悬空引用。
struct LifecycleStats {
    // 通过 LifecycleProbe(stats, value) 直接创建对象的次数。
    std::size_t direct_constructions{0};

    // 通过 LifecycleProbe(const LifecycleProbe&) 创建独立副本的次数。
    std::size_t copy_constructions{0};

    // 通过 LifecycleProbe(LifecycleProbe&&) 从另一个对象移动构造的次数。
    std::size_t move_constructions{0};

    // 生命周期结束并执行 ~LifecycleProbe() 的对象总数，包括移动后的源对象。
    std::size_t destructions{0};
};

// 用 LifecycleStats 观察构造、拷贝构造、移动构造和析构发生的时机。
//
// LifecycleProbe 自身不拥有统计器，只保存一个引用；value_ 是该对象独立拥有的整数状态。
// 这个类主要用于把 C++ 隐式发生的生命周期事件变成可检查、可测试的显式计数。
class LifecycleProbe {
public:
    // 直接构造：绑定调用者的统计器、保存 value，并增加 direct_constructions。
    // stats 不是可空参数，调用者必须保证它在本对象析构前一直有效。
    LifecycleProbe(LifecycleStats& stats, int value) noexcept;

    // 析构时不手工释放外部资源，只在共享统计器中增加 destructions。
    ~LifecycleProbe() noexcept;

    // 拷贝构造：新对象沿用源对象引用的同一统计器，并复制源对象的整数值。
    LifecycleProbe(const LifecycleProbe& other) noexcept;

    // 移动构造：新对象取得源对象当前的整数值；本练习随后把源值置为 0，
    // 以便从测试和输出中观察“已移动”状态。
    LifecycleProbe(LifecycleProbe&& other) noexcept;

    // stats_ 是引用成员，构造后不能改为引用另一份统计器。删除两个赋值运算符，
    // 可以避免“究竟应沿用目标统计器还是改用源统计器”的含糊语义。
    LifecycleProbe& operator=(const LifecycleProbe&) = delete;
    LifecycleProbe& operator=(LifecycleProbe&&) = delete;

    // 只读访问当前整数状态。const 保证不修改对象，noexcept 表示不会抛出异常；
    // [[nodiscard]] 提醒调用者不要无意中丢弃读取结果。
    [[nodiscard]] int value() const noexcept;

private:
    // 非拥有引用：实际 LifecycleStats 的创建、存活和销毁均由调用者负责。
    LifecycleStats& stats_;

    // 当前对象独立拥有的简单状态；拷贝时复制，移动时转移其可观察值。
    int value_;
};

// 作用域计时器：利用 RAII 把“开始计时”和“结束计时”绑定到对象生命周期。
//
// 构造函数在进入目标作用域时记录起点；无论作用域正常结束、提前 return，还是因异常
// 展开而退出，析构函数都会尝试计算持续时间并写入输出流。因此调用者不需要在每条退出
// 路径上手工调用 stop()。输出流由调用者拥有，必须覆盖 ScopeTimer 的整个生命周期。
class ScopeTimer {
public:
    // label 按值接收，便于把实参移动到成员中；output 仅在计时器存活期间被借用。
    ScopeTimer(std::string label, std::ostream& output);

    // 析构时输出微秒级耗时。析构函数承诺不把异常传播给外部。
    ~ScopeTimer() noexcept;

    // 一个 ScopeTimer 只代表一个明确的作用域和一个开始时刻。禁止拷贝与移动可避免
    // 同一计时区间被多个对象重复输出，也避免移动后对象的输出流指针语义变复杂。
    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;
    ScopeTimer(ScopeTimer&&) = delete;
    ScopeTimer& operator=(ScopeTimer&&) = delete;

private:
    // steady_clock 不受系统时间校准或人工修改影响，适合计算两个时刻的时间差。
    using Clock = std::chrono::steady_clock;

    // 计时标签由 ScopeTimer 自己拥有，因此调用者传入的临时字符串也可以安全使用。
    std::string label_;

    // 非拥有指针：使用指针保存可重新访问的输出流地址，但绝不 delete 它。
    // 构造参数是引用，所以这里不会得到 nullptr。
    std::ostream* output_;

    // 构造完成时记录的起始时刻，析构时用 Clock::now() 与它相减。
    Clock::time_point start_;
};

// 在动态存储期创建 LifecycleProbe，并把唯一所有权交给返回的 unique_ptr。
//
// 调用者接收返回值后负责持有或通过 std::move 转移所有权；当最后一个 unique_ptr
// 被销毁或 reset() 时，LifecycleProbe 会自动析构，不需要手工调用 delete。
// 内存分配可能失败并抛出异常，因此本函数没有声明 noexcept。
[[nodiscard]] std::unique_ptr<LifecycleProbe> make_owned_probe(
    LifecycleStats& stats,
    int value);

}  // namespace week1_wed_task_a：结束周三任务 A 的接口命名空间。
