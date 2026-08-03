// 优先包含与本实现对应的头文件，可以尽早发现头文件缺少必要 include 的问题。
#include "task_a/cpp_basics.hpp"

#include <numeric>  // std::accumulate
#include <utility>  // 本练习涉及移动语义；统一保留相关标准工具声明。

// 实现仅属于练习区，不加入正式的 aerial_core 库。
namespace task_a {

void swap_by_reference(int& left, int& right) noexcept {
    // 必须先保存 left 的旧值，否则直接执行 left = right 会把旧值覆盖掉。
    // temporary 是函数拥有的局部对象，只需存活到交换完成。
    const int temporary = left;

    // left 和 right 都是调用者对象的别名，给引用赋值会修改它们指向的原对象，
    // 而不是让引用改为绑定另一个对象。
    left = right;
    right = temporary;
}

double sum(const std::vector<double>& readings) noexcept {
    // std::accumulate 从初始值 0.0 开始，依次把 [begin, end) 范围内的元素相加。
    // 初始值写成 double 类型的 0.0，可以确保累加过程使用浮点运算。
    // 空 vector 的 begin() 等于 end()，循环不会执行，因此自然返回 0.0。
    return std::accumulate(readings.begin(), readings.end(), 0.0);
}

std::optional<double> mean(const std::vector<double>& readings) noexcept {
    // 平均值需要除以元素个数。必须先处理空容器，既避免除以零，也明确表达
    // “空数据集没有平均值”，而不是使用 0.0 之类可能被误解的哨兵值。
    if (readings.empty()) {
        return std::nullopt;
    }

    // 复用 sum()，避免在两个函数中重复求和逻辑。
    // readings.size() 的类型是 std::size_t；显式转换为 double 后再做除法，
    // 可以清楚表达这里需要浮点除法，最终结果被包装进 std::optional<double>。
    return sum(readings) / static_cast<double>(readings.size());
}

std::size_t copies_seen_by_value(CopyTracked object) noexcept {
    // object 是本函数拥有的参数对象。如果调用者传入左值，调用前会调用拷贝构造函数，
    // 因而这里观察到的 copy_count 比调用者原对象多 1。函数返回后该副本被销毁。
    return object.copy_count;
}

std::size_t copies_seen_by_const_reference(const CopyTracked& object) noexcept {
    // object 只是调用者对象的只读别名，没有创建新的 CopyTracked，因此计数不增加。
    return object.copy_count;
}

bool increment_if_not_null(int* value) noexcept {
    // 裸指针可以保存 nullptr。任何解引用操作前都必须完成判空，否则会产生未定义行为。
    if (value == nullptr) {
        return false;
    }

    // *value 取得指针所指向的调用者对象，前置 ++ 直接修改该对象。
    // 函数并不 delete value，因为它只是借用指针，不拥有对象。
    ++(*value);
    return true;
}

Vec3 scale_vec3(const Vec3& vector, const double scale) noexcept {
    // 花括号构造一个新的 std::array<double, 3> 返回值。每个输出分量都由对应输入分量
    // 乘以标量得到；vector 是 const 引用，所以原数组保持不变。
    return {
        vector[0] * scale,
        vector[1] * scale,
        vector[2] * scale,
    };
}

std::unordered_map<std::string, std::size_t> count_strings(
    const std::vector<std::string>& words) {
    // counts 是函数拥有的局部哈希表。返回时编译器通常通过返回值优化直接在调用者位置
    // 构造它；即使没有优化，也可以高效移动，不需要调用者管理其内部内存。
    std::unordered_map<std::string, std::size_t> counts;

    // const std::string& 只读引用 vector 中已有的字符串，避免循环的每次迭代都复制字符串。
    for (const std::string& word : words) {
        // operator[] 在键不存在时插入该字符串，并把 std::size_t 值初始化为 0；
        // 随后的前置 ++ 将计数加一。哈希表自己保存一份键，不依赖 word 引用长期存活。
        ++counts[word];
    }

    // 按值返回完整结果，所有权交给调用者。
    return counts;
}

std::unique_ptr<int> make_owned_int(const int value) {
    // make_unique 在动态存储期构造 int，并立即用 unique_ptr 管理它。
    // 与手写 new 相比，即使后续代码抛出异常，也更不容易泄漏资源。
    return std::make_unique<int>(value);
}

std::optional<int> consume_owned_int(std::unique_ptr<int> owned) noexcept {
    // unique_ptr 也可以为空。先检查所有权是否存在，再进行解引用。
    if (!owned) {
        return std::nullopt;
    }

    // *owned 读取被管理的整数，并按值复制到 optional 中。
    // return 表达式完成后，局部参数 owned 被析构，动态整数自动释放；返回值不依赖它。
    return *owned;
}

}  // namespace task_a
