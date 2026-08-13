# 8 月 7 日（周五）任务 E：第一次集成检查

验证日期：2026-08-07。

## 1. 验证环境

- 操作系统：Windows；
- C++ 编译器：MinGW GCC/G++ 15.2.0（UCRT）；
- CMake：3.28.3 便携版；
- 生成器：MinGW Makefiles；
- Python：3.12.4（Anaconda）；
- pytest：7.4.4；
- NumPy：1.26.4。

Windows 是本周的次级验证环境；Ubuntu GCC 13.3 主环境仍需在对应电脑上执行相同 preset，本记录不以 Windows 结果替代 Linux 实机验证。

## 2. 干净构建

本次使用此前不存在的新目录：

```text
build/week1-friday-clean
```

配置等价于项目的 Windows preset，启用 C++17、Eigen、GoogleTest、CTest 和每周练习。实际识别结果为：

```text
host     = Windows
compiler = GNU 15.2.0
CMake    = 3.28.3
Eigen    = 3.4.1
```

配置成功，全部 target 构建成功。构建输出没有出现项目代码或第三方依赖的编译警告。

## 3. C++ 测试结果

执行：

```text
ctest --test-dir build/week1-friday-clean --output-on-failure
```

结果：

```text
通过 12 / 总计 12
失败 0
```

其中新增的 `RotationUtilitiesTest` 通过 7/7，既有 Eigen、冒烟测试、周二和周三练习测试也全部通过。

## 4. Python 测试结果

执行：

```powershell
$env:PYTHONPATH = "python"
python -m pytest python/tests -v
```

结果：

```text
通过 40 / 总计 40
失败 0
```

Python 四元数用例与 C++ 用例共同遵循 [`docs/rotation_test_vectors.md`](../../../../../docs/rotation_test_vectors.md)。

## 5. 集成结论

- `aerial_core` 已包含 C++/Eigen 旋转工具；
- GoogleTest 覆盖单位旋转、单轴 90°、组合、正逆变换、归一化和 `q/-q` 等价；
- Python/C++ 共享固定测试向量；
- README 已增加旋转模块和测试命令；
- 新建构建目录受 `.gitignore` 规则保护，不纳入版本控制；
- 本次集成在 Windows MinGW GCC 环境通过，未发现剩余编译警告。
