#include "week1_wed_task_a/lifecycle_raii.hpp"  // 优先包含自身接口，及时发现头文件缺失依赖。

#include <chrono>   // std::chrono::duration_cast、std::chrono::microseconds
#include <memory>   // std::make_unique
#include <ostream>  // std::ostream 的完整定义与 operator<<
#include <utility>  // std::move

// 实现放在与头文件一致的命名空间中，避免符号进入全局命名空间。
namespace week1_wed_task_a {

LifecycleProbe::LifecycleProbe(LifecycleStats& stats, const int value) noexcept
    : stats_(stats), value_(value) {
    // 引用成员 stats_ 必须在初始化列表中完成绑定，不能先默认构造再赋值。
    // 两个成员初始化完成后对象已经具备有效状态，此时记录一次直接构造事件。
    ++stats_.direct_constructions;
}

LifecycleProbe::~LifecycleProbe() noexcept {
    // 每个成功构造的对象最终都会执行一次析构，包括被移动后 value_ 已变为 0 的源对象。
    // stats_ 不由本对象拥有，所以这里只更新计数，绝不能释放或销毁外部统计器。
    ++stats_.destructions;
}

LifecycleProbe::LifecycleProbe(const LifecycleProbe& other) noexcept
    : stats_(other.stats_), value_(other.value_) {
    // 新对象引用与源对象相同的统计器，但 value_ 是按值复制的独立成员。
    // 后续即使其中一个对象的 value_ 改变，也不会直接改变另一个对象的 value_。
    ++stats_.copy_constructions;
}

LifecycleProbe::LifecycleProbe(LifecycleProbe&& other) noexcept
    : stats_(other.stats_), value_(other.value_) {
    // 对本练习中的 int 而言，“移动”底层仍是一次整数复制；区别在于该构造函数接收右值引用，
    // 并明确把源对象改成可观察的已移动状态。真实资源类通常会在这里转移句柄或指针。
    ++stats_.move_constructions;

    // C++ 只要求移动后的源对象仍然有效、可以析构，通常不保证其具体值。
    // 本类主动约定源对象 value_ 变为 0，只是为了让示例和测试能清楚观察移动结果。
    other.value_ = 0;
}

int LifecycleProbe::value() const noexcept {
    // 按值返回 int，不暴露成员引用，调用者无法借此修改对象内部状态。
    return value_;
}

ScopeTimer::ScopeTimer(std::string label, std::ostream& output)
    : label_(std::move(label)), output_(&output), start_(Clock::now()) {
    // label 是按值参数，std::move 可把它已有的字符缓冲区转交给 label_，避免不必要的深拷贝。
    // output_ 只保存调用者输出流的地址；start_ 使用单调时钟记录构造阶段的当前时刻。
}

ScopeTimer::~ScopeTimer() noexcept {
    // 先取得结束时刻，再减去构造时保存的 start_，得到稳定、不受系统时钟跳变影响的时长。
    // duration_cast 把时钟的原生精度统一转换成微秒，便于输出和测试格式。
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_);

    // 析构函数声明为 noexcept。如果流已配置为失败时抛异常，直接让异常逃出析构函数会违反
    // noexcept；尤其在另一个异常展开栈的过程中，这会触发 std::terminate 并终止程序。
    try {
        // output_ 在构造时来自有效引用；调用者负责保证输出流此时仍然存活。
        *output_ << "[ScopeTimer] " << label_ << ": " << elapsed.count() << " us\n";
    } catch (...) {
        // 计时信息属于辅助诊断。即使磁盘、字符串流或其他输出目标失败，也不应改变
        // 被计时代码原本的返回或异常传播路径，因此这里有意吞掉所有输出异常。
    }
}

std::unique_ptr<LifecycleProbe> make_owned_probe(LifecycleStats& stats, const int value) {
    // make_unique 在动态存储期构造 LifecycleProbe，并立即用 unique_ptr 接管它。
    // 与手写 new 相比，即使后续代码抛出异常，所有权也始终由 RAII 对象明确管理。
    // 返回 unique_ptr 会转移唯一所有权；现代 C++ 通常还会进行返回值优化。
    return std::make_unique<LifecycleProbe>(stats, value);
}

}  // namespace week1_wed_task_a
