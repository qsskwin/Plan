# 开发环境

## 唯一主环境

本项目固定使用当前 Linux 工作区作为主开发环境，不在 Windows 原生、WSL 或其他虚拟机之间切换。第一周不迁移环境，也不主动升级工具链。

记录日期：2026-08-01。

| 项目 | 版本 |
| --- | --- |
| 操作系统 | Ubuntu 24.04.4 LTS (Noble Numbat) |
| C++ 编译器 | GCC/G++ 13.3.0 (`c++ (Ubuntu 13.3.0-6ubuntu2~24.04.1)`) |
| CMake | 3.28.3 |
| Git | 2.43.0 |
| Python | 3.12.3 |

## 最小构建命令

在仓库根目录执行：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

运行最小应用：

```bash
./build/sanity_check
```

以上命令只使用系统中已有的工具，不要求安装第三方 C++ 库。
