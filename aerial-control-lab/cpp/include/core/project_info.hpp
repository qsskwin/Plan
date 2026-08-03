#pragma once 
//等于 #ifndef PROJECT_INFO_HPP
//等于 #define PROJECT_INFO_HPP
//等于 #endif pragma = paragmatic 

#include <string_view>

namespace aerial_control {

/// Returns the stable project identifier used by smoke checks.
std::string_view project_name() noexcept;

}  // namespace aerial_control no exception  没有异常，声明该函数不会抛出异常。
