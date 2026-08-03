#include <iostream>
#include <string_view>

#include "core/project_info.hpp"

int main() {
    constexpr std::string_view expected_name{"aerial-control-lab"};
    const std::string_view actual_name = aerial_control::project_name();

    if (actual_name != expected_name) {
        std::cerr << "Expected project name '" << expected_name
                  << "', got '" << actual_name << "'.\n";
        return 1;
    }

    return 0;
}

// constexpr 代表编译期常量。编译器在编译阶段就能确定 expected_name 的值，不占用运行时计算开销。
// 只能用于编译时就能确定结果的数据。const expresstion