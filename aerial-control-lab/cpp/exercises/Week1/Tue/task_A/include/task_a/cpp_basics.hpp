#pragma once  // 防止同一个翻译单元重复包含本头文件，避免重复定义。

#include <array>          // std::array：编译期固定长度的连续数组。
#include <cstddef>        // std::size_t：用于表示非负的大小和计数。
#include <memory>         // std::unique_ptr：表达独占所有权的智能指针。
#include <optional>       // std::optional：表达“可能没有结果”。
#include <string>         // std::string：拥有字符数据的动态字符串。
#include <unordered_map>  // std::unordered_map：基于哈希表的键值容器。
#include <vector>         // std::vector：运行时可变长度的连续数组。

// Week1 / Tue / task_A：值、引用、指针、STL 与所有权练习接口。
// 命名空间把练习符号与正式的 aerial_core 接口隔离，避免全局命名冲突。
namespace task_a {

// 练习 1：使用引用交换两个整数。
//
// left 和 right 是调用者对象的别名，函数不拥有这两个对象，也不会延长其生命周期。
// 引用不能表示“没有对象”，因此调用者必须传入两个有效的 int。参数没有 const，表示函数
// 被允许修改调用者的原对象；调用结束后，调用者能够直接看到交换结果。
// noexcept 表示该函数承诺不会抛出异常。
void swap_by_reference(int& left, int& right) noexcept;

// 练习 2：通过 const 引用只读访问动态数组。
//
// readings 仍由调用者拥有，函数只在调用期间借用它。使用引用避免复制整个 vector，
// 使用 const 则从类型层面禁止函数增加、删除或修改其中的元素。
// sum 对空 vector 返回 0.0；mean 对空 vector 没有合法平均值，因此返回 std::nullopt。
double sum(const std::vector<double>& readings) noexcept;
std::optional<double> mean(const std::vector<double>& readings) noexcept;

// 练习 3：记录一个对象经历过的复制次数。
//
// 每个 CopyTracked 对象直接拥有自己的 copy_count。新建对象从 0 开始；从另一个对象复制时，
// 新副本把计数设为“源对象计数 + 1”。这样可以观察值传递是否触发了拷贝构造。
struct CopyTracked {
    // 成员使用花括号提供默认值，因此默认构造出的对象计数为 0。
    std::size_t copy_count{0};

    // 默认构造函数不需要执行额外逻辑，由编译器生成即可。
    CopyTracked() = default;

    // 拷贝构造函数接收 const 引用：读取源对象但不修改它，然后创建独立的新对象。
    CopyTracked(const CopyTracked& other) noexcept
        : copy_count(other.copy_count + 1) {}

    // 拷贝赋值用于“两个对象都已存在”的场景。先防止自赋值，再根据源对象更新当前对象。
    // 返回 *this 的引用符合 C++ 赋值运算符惯例，也支持 a = b = c 形式的连续赋值。
    CopyTracked& operator=(const CopyTracked& other) noexcept {
        if (this != &other) {
            copy_count = other.copy_count + 1;
        }
        return *this;
    }

    // 移动表示转移而不是复制，因此沿用编译器生成的行为，不增加 copy_count。
    CopyTracked(CopyTracked&&) noexcept = default;
    CopyTracked& operator=(CopyTracked&&) noexcept = default;
};

// object 按值传递：使用左值调用时，进入函数前会创建一个属于函数参数的副本。
// 返回参数对象记录的计数，可以直接观察这次复制。
std::size_t copies_seen_by_value(CopyTracked object) noexcept;

// object 按 const 引用传递：函数只借用原对象，不创建副本，也不能修改它。
std::size_t copies_seen_by_const_reference(const CopyTracked& object) noexcept;

// 练习 4：安全使用可能为空的裸指针。
//
// value 是非拥有指针：整数的所有权仍在调用者手中，且被指向对象必须覆盖整个函数调用。
// 指针与引用不同，可以是 nullptr，所以函数必须先判空。非空时函数把整数加一并返回 true；
// 为空时不解引用，直接返回 false。参数指向非 const int，因此函数可以修改该整数。
bool increment_if_not_null(int* value) noexcept;

// 练习 5：使用固定长度数组表示三维数据。
//
// std::array<double, 3> 的三个元素直接存储在 Vec3 对象内部，不进行单独的动态分配；
// 数组元素与 Vec3 对象具有相同生命周期，长度在编译期固定为 3。
using Vec3 = std::array<double, 3>;

// vector 由 const 引用只读借用，scale 按值传递；函数返回一个由调用者接收的新 Vec3，
// 因此不会原地修改输入数组。
Vec3 scale_vec3(const Vec3& vector, double scale) noexcept;

// 练习 6：使用哈希表统计字符串出现次数。
//
// words 由调用者拥有，函数通过 const 引用只读遍历。返回的 unordered_map 会独立拥有
// std::string 键和计数，所以函数返回后不依赖输入 vector 继续存活。
// 该函数可能在容器扩容或复制字符串时分配内存，因此没有声明 noexcept。
std::unordered_map<std::string, std::size_t> count_strings(
    const std::vector<std::string>& words);

// 练习 7 的悬空引用错误示例只保存在 docs/dangling_reference_bad_example.md 中，
// 不声明为正式接口，也不会参与编译，避免程序主动执行未定义行为。

// 练习 8：使用 std::unique_ptr 表达唯一所有权。
//
// make_owned_int 在动态存储期创建一个 int，并把唯一所有权交给返回的 unique_ptr。
// 分配内存可能失败并抛出异常，所以本函数不声明 noexcept。
std::unique_ptr<int> make_owned_int(int value);

// owned 按值接收 unique_ptr，表示调用者必须通过 std::move 明确转移所有权。
// 非空时返回整数值；空指针时返回 std::nullopt。函数结束时 owned 析构，它所拥有的
// 动态整数随之自动释放。返回的是 int 的值而不是引用，因此不会产生悬空引用。
std::optional<int> consume_owned_int(std::unique_ptr<int> owned) noexcept;

}  // namespace task_a：到此结束练习接口的命名空间。
