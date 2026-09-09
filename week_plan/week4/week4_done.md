# 第 04 周进度汇报（2026-08-31 至 2026-09-06）

> 总体路线：[总体规划](../../总体规划.md)  
> 本周计划：[第 04 周执行计划](./第04周计划_2026-08-31至2026-09-06.md)  
> 项目目录：[aerial-control-lab](../../aerial-control-lab/)

## 当前执行说明

- 实际开始记录日期：2026-09-04。
- 当前按原顺序补做周一独立复验门槛，不把后续日程压缩到同一天。

## 2026-08-31（周一）滚动记录

### 任务 A：确认基线

- C++ 构建与全量 CTest：84/84 通过。
- Python 全量 pytest：99/99 通过。
- Python 解释器：`D:\anaconda\python.exe`。
- 验证时仓库 HEAD：`0e17b8b`。
- 验证时源码无改动，`week_plan/week4/` 尚未被 Git 跟踪。
- 本次命令由 Codex 执行，仅作为环境基线证据，不作为本人独立操作证据。

### 任务 B：最小 RK4 独立重写

| 检查点 | 当前评级 | 证据与边界 |
|---|---|---|
| RK4 四阶段采样时间和预测状态关系 | H | 经提示后正确写出 `t`、`t+h/2`、`t+h/2`、`t+h`，以及各阶段预测状态表达式。 |
| 区分采样状态与状态导数 | H | 首次手算将采样状态误写为 `k`；经提示后能够指出 `k=f(t,y)` 是状态导数。 |
| RK4 简谐振子单步数值手算 | G | 用户决定停止手算，后续数值由 Codex 直接给出；该部分不能作为独立掌握证据。 |
| Python 最小 RK4 重写 | H | 在公式提示后从空文件正确实现简谐振子导数与 RK4 单步；四个阶段均通过导数函数求值，输入状态未被修改。 |
| C++ 最小 RK4 重写 | H | 本人根据公式完成四阶段核心；经提示修正 Eigen 动态向量构造、已使用/未使用时间参数及相应编译错误，严格语法检查通过。 |

- 本轮“RK4 四阶段概念理解”记录为 **H**。
- Python 独立重写验证：单步 `dt=0.1` 的最大绝对误差约为 `8.33135e-08`；连续 10 步到 `t=1` 的最大绝对误差约为 `6.61249e-07`；输入数组保持不变。
- Python 正式对比程序已完成：本人实现与正式实现的最终状态相同，跨实现最大绝对差为 `0.0`；两者相对解析解的最大绝对误差均约为 `6.61249e-07`。
- 对比阶段曾把“正式实现与解析解”误写成跨实现比较，经提示定位并修正，相关过程保持 **H**。
- Python 最小 RK4 重写与正式积分器对比已达到 **H**。
- C++ RK4 核心实现记录为 **H**；`simulate` 的初版由本人完成，解析解、标签化输出、误差计算、函数层次整理及格式收口由 Codex 直接补齐，相关局部记为 **G**。
- C++ CMake target 与根目录接入由本人在提示后完成，记为 **H**；target 正确链接 `aerial_core` 并继承公共头文件路径、C++17 要求和 Eigen 依赖。
- C++ 已完成正式配置、链接和运行：本人实现与正式实现的最终状态相同，跨实现最大绝对差为 `0`；两者相对解析解的最大绝对误差均约为 `6.61249e-07`。
- 本次构建没有项目源码警告；配置阶段仅有 Eigen 上游已有的 CMake 策略弃用提示。
- Python/C++ 双语言最小 RK4 重写门槛通过，核心评级均为 **H**。

### 任务 C：坐标与推力方向验证

| 检查点 | 当前评级 | 证据与边界 |
|---|---|---|
| NED→ENU→NED 方向推导 | I | 独立给出 `[1,2,3] -> [2,1,-3] -> [1,2,3]`。 |
| `R_NB^T` 为逆变换 | I | 独立使用旋转矩阵正交性 `R^T R=I` 和逆矩阵定义说明 `R^T=R^{-1}`。 |
| 正滚转推力方向 | H | 在补充零姿态轴对齐、右手正角和 `R_NB` 为 B→N 后，得到负 body-z 推力的 North 为零、East 为正、Down 为负。 |
| Week4 GoogleTest | 跳过/未验证 | 用户明确决定跳过；不记录为自动化测试通过，也不增加测试数量。 |
| 最小 Python 数值验证脚本 | G | 用户明确要求 Codex 直接完成；脚本覆盖 NED/ENU 往返、模长、`R_NB^T` 恢复和正滚转推力方向。 |

- 数值结果：NED/ENU 往返最大误差 `0.0`，模长差 `0.0`，`R_NB^T` 恢复最大误差约 `4.44089e-16`，正滚转推力与解析值最大差约 `1.11022e-16`。
- 周一坐标方向的概念理解整体记录为 **H**；最小数值脚本保持 **G**，GoogleTest 仍为跳过/未验证，不能将生成的脚本记为独立实现。

### 任务 D：小范围代码卫生

- 将 `cpp/tests/integrators_test.cpp` 中误拼的辅助函数 `oscillatorErroeulerStepr` 重命名为 `oscillatorError`。
- 本人根据明确提示使用符号重命名完成 1 处定义和 6 处调用的替换，记为 **H**。
- diff 仅包含名称及对应引用，没有改动函数体、测试参数或正式积分器源码。
- `aerial_core_gtest` 定向构建成功，积分器相关 CTest `17/17` 通过。

### 任务 E：复习 438

- 438. 找到字符串中所有字母异位词：用户报告已 AC，自评为 **A**。
- 实际用时未提供。
- 固定窗口边界、进出窗口更新和频次数组相等的正确性解释由用户明确跳过，因此 A 级核心不变量未验收。
- 记录口径：**A（用户自评，解释未验证）**。

### 周一门槛小结

- Python/C++ 最小 RK4 核心均为 **H**，正式对比通过。
- 坐标和推力方向的概念理解整体为 **H**；GoogleTest 跳过，数值验证脚本为 **G**。
- 小范围重命名为 **H**，相关测试 `17/17` 通过。
- 438 已 AC、自评 A，但核心不变量解释跳过。
- 执行分支：允许进入周二 Python 三维质点平动；上述跳过项和 G 来源继续保留，不补记为独立证据。

## 2026-09-01（周二）滚动记录

### 任务 A：六维状态推导

| 检查点 | 当前评级 | 证据与边界 |
|---|---|---|
| 六维状态顺序、单位和导数结构 | H | 能说明位置单位为 m、速度单位为 m/s，并写出 `dx=[v_n,v_e,v_d,a_n,a_e,a_d]`；`v_e` 曾误写为 `n_e`，经指出后完成收口。 |
| 单位姿态、零推力 | I | 独立得到 NED 加速度 `[0,0,g]`，并说明 Down 轴向下为正，因此自由落体的 `a_d` 为正。 |
| 单位姿态、`T=mg` | H | 经分步提示，将 FRD 推力 `[0,0,-mg]` 除以质量得到 `[0,0,-g]`，与 NED 重力相加得到 `[0,0,0]`。 |
| 固定正滚转 | H | 在给出绕 x 轴的 `R_NB` 及矩阵乘法提示后，得到 `f_T^N=[0,T sin(phi),-T cos(phi)]`；代入 `T=mg/cos(phi)` 得到 `a_N=[0,g tan(phi),0]`，判断为 NED 正 East 加速。计算中一度漏写 Down 分量的 `T`，经单位提示后修正。 |

- 周二任务 A 已完成，整体记录为 **H**，本项无 `G` 内容。
- 三个工况的单步加速度预测：自由落体仅有正 Down 分量；水平悬停三轴均为零；固定正滚转仅有正 East 分量。
- 本阶段只完成纸面推导和解释验收；尚未开始 `point_mass_derivative`，也未新增或运行周二代码与测试。

### 任务 B：Python 核心实现

- 新增 `python/aerial_control/point_mass.py`，本人依据接口模板和分步提示完成 `point_mass_derivative`，整体记录为 **H**。
- 已实现六维状态形状与有限性检查、正且有限的质量检查、非负且有限的推力检查；四元数形状、有限性、归一化和零范数拒绝复用现有 `quat_rotate_vector`。
- 动力学函数复用 FRD→NED 四元数旋转，返回 `[v_n,v_e,v_d,a_n,a_e,a_d]`，无内部时间循环、绘图或文件输出。
- 首版曾把 NED 重力写成负号，定向运行出现自由落体 `a_d=-g`、悬停 `a_d=-2g`；经提示依据 NED Down 为正修正为重力向量 `[0,0,g]`。
- 首版质量和推力只检查取值范围，未拒绝 NaN/Inf；经 `np.isfinite` 提示补齐，保持 **H**。
- Codex 定向运行结果：位置导数 `[4,5,6]`；自由落体加速度 `[0,0,9.80665]`；水平悬停 `[0,0,0]`；固定正滚转 `[0,1.72917699,0]`，解析 East 加速度约 `1.729176985364668`。
- 质量 0、负数、NaN、Inf，推力负数、NaN、Inf，错误状态维度、非有限状态，以及零范数、非有限、错误维度四元数均实际抛出 `ValueError`。
- 上述为临时定向运行证据；任务 C 的正式 pytest 尚未编写或计入测试数量。

### 任务 C：Python 首批核心测试

- 本次已完成：本人在框架和 pytest API 提示后编写首个正式测试 `test_position_derivative_equals_velocity`，记录为 **H**；尚未完成任务 C 的其余物理与非法输入测试。
- 测试使用三个非零速度，断言返回形状为 `(6,)`，并用 `np.testing.assert_array_equal` 验证导数前三项等于输入速度。
- 测试中的 `q_nb=[0,0,0,1]` 是合法单位范数四元数但不是单位姿态；位置导数与姿态无关，因此不影响本测试，后续自由落体与悬停使用单位姿态 `[1,0,0,0]`。
- Codex 本段只提供测试命名、结构和断言 API 提示，没有直接生成或修改测试代码，无 `G` 变体。
- 实际命令：从 `aerial-control-lab` 根目录设置 `PYTHONPATH` 为 `python`，使用 `D:\anaconda\python.exe -m pytest .\python\tests\test_point_mass.py -q`。
- 实际结果：正式 pytest 收集并通过 1 项，`1 passed in 0.15s`。
- 下一项：本人编写“零推力只产生 NED 正 Down 重力”的首个物理工况测试。
- 后续完成：本人编写 `test_zero_thrust_produces_ned_gravity`，使用单位姿态、零推力和手写解析期望 `[0,0,9.80665]`，以 `rtol=0`、`atol=1e-12` 验证加速度，记录为 **H**。
- 再次运行同一正式 pytest 文件，实际结果为 `2 passed in 0.13s`；下一项为单位姿态下 `T=mg` 的水平悬停测试。
- 后续完成：本人编写 `test_hover_thrust_cancels_ned_gravity`，使用 `m=1.5 kg`、`T=m*STANDARD_GRAVITY_MPS2` 和单位姿态验证三轴加速度为零，记录为 **H**。
- 再次运行同一正式 pytest 文件，实际结果为 `3 passed in 0.13s`；下一项为固定正滚转的 East 加速度测试。
- 正滚转首版正式测试失败，结果为 `1 failed, 3 passed in 0.17s`：四元数直接使用 `cos(phi),sin(phi)`，实际形成两倍滚转角，同时推力仍取 `mg`；经提示区分四元数半角与实际姿态角，并要求使用 `T=mg/cos(phi)`。
- 第二版仍为 `1 failed, 3 passed in 0.16s`：四元数改用 5° 半角后正确表示 10° 滚转，但推力补偿和解析 `tan` 也误用了 5°；下一步统一定义实际角 `phi=10°`，仅四元数分量使用 `phi/2`。
- 用户随后明确要求 Codex 直接修正；Codex 修改了实际角变量、四元数半角、`T=mg/cos(phi)`、解析期望和容差，这些直接修改记为 **G**。用户原先编写的测试结构与两次调试尝试保留为 **H** 过程证据，最终正滚转测试来源记录为 H/G 混合，不补记为完整独立实现。
- 修正后正式 pytest 结果为 `4 passed in 0.14s`；下一项为质量、推力、状态和四元数的代表性非法输入测试。
- 本人随后使用 `pytest.mark.parametrize` 编写非法质量测试，覆盖 0、负数、NaN、正负 Inf，五个参数分别被收集，记录为 **H**。
- 正式 pytest 结果为 `9 passed in 0.13s`。测试行为正确；`pytest.raises` 当前还包住了状态构造，后续将异常上下文缩小到被测函数调用，避免准备代码异常造成误通过。
- 下一项：本人编写负推力与非有限推力的参数化测试。

- 本人完成 `test_invalid_total_thrust_is_rejected`，参数化覆盖负推力、NaN、正负 Inf；其他输入保持合法，记录为 **H**。
- 此前非法质量测试已按提示把状态构造移出 `pytest.raises`，异常上下文只覆盖被测函数调用。
- 实际命令：从 `aerial-control-lab` 根目录设置 `PYTHONPATH` 为 `python`，使用 `D:\anaconda\python.exe -m pytest .\python\tests\test_point_mass.py -q`。
- 实际结果：正式 pytest 收集并通过 13 项，`13 passed in 0.14s`。
- 下一项：状态维度、状态有限性和输入状态不被修改。
- 本人完成 `test_invalid_state_is_rejected`，参数化覆盖长度 5、长度 7、NaN、正 Inf 和负 Inf，五项均通过，记录为 **H**。
- 正式 pytest 当前结果为 `18 passed in 0.14s`。二维形状 `(2,3)` 尚未覆盖，输入状态不变性测试也尚未编写。
- 本人补充二维状态 `(2,3)`，并新增 `test_call_does_not_modify_input_state`，通过 `state.copy()` 与调用后原数组比较验证输入不被修改，记录为 **H**。
- 正式 pytest 当前结果为 `20 passed in 0.16s`；状态形状、有限性和输入不变性覆盖完成，下一项为四元数非法输入及非单位四元数归一化行为。
- 用户明确要求 Codex 完成剩余四元数测试并要求记为 H；依据既定来源规则，Codex 直接新增的 `test_invalid_quaternion_is_rejected` 和 `test_non_unit_quaternion_is_normalized` 必须记录为 **G**，不能改记为 H。
- 生成测试覆盖错误形状、零范数、NaN、Inf 四元数，并比较单位四元数与其 3 倍缩放后的完整导数，确认现有旋转模块的归一化行为。
- 固定解释器经实际查询为 `D:\anaconda\python.exe`；正式命令运行结果为 `25 passed in 0.15s`。
- 周二任务 C 的行为覆盖已经完成，来源为 H/G 混合：位置导数、自由落体、悬停、质量/推力/状态非法输入及输入不变性为本人在提示后完成的 H；正滚转最终修正和两类四元数测试含 Codex 直接代码，保持 G 边界。
- 本次只运行 `python/tests/test_point_mass.py`；Python 全量回归留到计划中的周六验收，不把定向结果替代全量结果。

### 任务 D：复习 15

- 15. 三数之和：用户报告已 AC，自评为 **B**。
- 实际用时未提供；先排序、双指针搜索区间与移动条件、外层/左侧/右侧三处去重及复杂度解释尚未验收。
- 当前记录口径：**B（用户自评，解释未验证）**。
- 用户明确选择跳过本次排序、双指针移动、三处去重和复杂度解释验收；保持“解释未验证”，不补记为通过。

### 周二任务小结

- 任务 A 六维状态与三个工况推导已完成，整体为 **H**。
- 任务 B `point_mass_derivative` 已完成并通过定向运行，整体为 **H**。
- 任务 C 正式 pytest 行为覆盖已完成，定向结果为 `25 passed in 0.15s`；来源为 H/G 混合，其中正滚转最终修正及两类四元数测试含 **G**，不改记为 H。
- 任务 D：15 已 AC，自评 B；正确性解释由用户明确跳过，保持未验证。
- 周二执行段已收口，允许进入周三 Python 仿真与可视化；上述 G 和跳过边界继续保留。

## 2026-09-02（周三）滚动记录

### 任务 A：独立仿真循环

- 新增 `python/aerial_control/simulation.py`，提供通用固定步长入口 `simulate_fixed_step(derivative, initial_state, t0, tf, dt)`；返回形状为 `(N+1,)` 的时间数组和 `(N+1, n)` 的状态历史，不包含质点工况、绘图或解析结果。
- 本人在下标与 API 提示后填写整数 `for` 核心循环：第 `k` 步使用 `times[k]`、`states[k]` 调用正式 `rk4_step`，结果保存到 `states[k+1]`，该核心循环记录为 **H**。
- 本人此前完成函数初版并尝试了浮点 `while` 与 Euler 更新；经指出不符合任务要求后移除。Codex 随后直接修正了闭包/状态/时间校验、整数步数与整除检查、时间数组和状态历史的预分配，这些直接写入部分记录为 **G**。因此当前文件整体来源为 H/G 混合，不把 G 支撑代码改记为 H。
- 时间由整数索引构造，先保存初始状态，执行 `N` 次更新并保存终点；初始状态要求一维、非空且有限，积分器继续负责导数及每步结果的形状和有限性检查。
- Codex 使用固定解释器 `D:\anaconda\python.exe` 进行了与既有周一/周二测试无关的二维常导数定向检查：`t0=1.0`、`tf=1.03`、`dt=0.01` 时得到 3 次更新、4 个状态，`times.shape==(4,)`、`states.shape==(4,2)`，终点正确、输入未修改、状态全程有限；非整除区间 `0→0.025`、`dt=0.01` 被拒绝。
- 上述是新增仿真入口的临时定向检查，不是正式 pytest，也没有重跑周一或周二测试。下一项为本人实现自由落体完整轨迹及逐时刻解析比较。

### 任务 B：三个固定工况

#### 自由落体

- 本人建立 `python/exercises/Week4/Wed/task_B/free_fall_trajectory.py`，使用六维零初值、`m=1.5 kg`、单位姿态、标量总推力 `T=0`、`t0=0`、`tf=2 s`、`dt=0.01 s`，并通过动力学闭包复用 `point_mass_derivative` 和公共仿真循环。
- 本人在明确解析公式后，依据 NumPy 切片与比较框架完成全部 201 个记录时刻的三维解析位置/速度构造及比较，记录为 **H**；NED 的 Down 为正，解析关系为 `p_d=0.5*g*t^2`、`v_d=g*t`，North/East 分量为零。
- Codex 直接加入的时间/状态形状、全程有限性、初始输入不变性断言与基础输出保持 **G**；当前自由落体脚本整体来源为 H/G 混合，不改变本人完成首个完整轨迹解析验证的 H 证据。
- 使用固定解释器 `D:\anaconda\python.exe` 和项目 `PYTHONPATH` 实际运行脚本：200 次更新、201 个状态，最终时刻 `2.00 s`，最终六维状态为 `[0, 0, 19.6133, 0, 0, 19.6133]`。
- 对每个记录时刻使用 `atol=1e-10`、`rtol=0` 比较：最大位置绝对误差为 `2.132e-14 m`，最大速度绝对误差为 `3.908e-14 m/s`，均通过；位置和速度误差分别保留单位。
- 本次只运行新增自由落体脚本，不是正式 pytest，也未运行周一、周二测试或 Python 全量回归。下一工况为水平悬停 10 s。

#### 水平悬停

- 本人沿用公共仿真循环建立 `python/exercises/Week4/Wed/task_B/hover_trajectory.py`，使用六维零初值、`m=1.5 kg`、单位姿态、标量总推力 `T=mg`、`t0=0`、`tf=10 s`、`dt=0.01 s`；NED 重力 `[0,0,+g]` 与由 FRD 推力 `[0,0,-T]` 得到的加速度抵消。
- 本人依据工况提示完成悬停参数、动力学闭包、全时刻零位置/零速度解析轨迹及比较，记录为 **H**；从自由落体脚本复用的 Codex 结构断言与输出框架保持 **G**，当前脚本整体来源为 H/G 混合。
- 使用固定解释器 `D:\anaconda\python.exe` 和项目 `PYTHONPATH` 实际运行脚本：1000 次更新、1001 个状态，最终时刻 `10.00 s`，最终六维状态为 `[0,0,0,0,0,0]`，时间与状态全程有限，初始输入未修改。
- 对每个记录时刻使用 `atol=1e-10`、`rtol=0` 比较：最大位置绝对误差为 `0.000e+00 m`，最大速度绝对误差为 `0.000e+00 m/s`，均通过。
- 本次只运行新增水平悬停脚本，不是正式 pytest，也未运行周一、周二测试或 Python 全量回归。下一工况为固定正滚转 10°、2 s。

#### 固定正滚转

- 本人沿用公共仿真循环建立 `python/exercises/Week4/Wed/task_B/positive_roll_trajectory.py`，使用六维零初值、`m=1.5 kg`、`phi=10°`、`q_NB=[cos(phi/2),sin(phi/2),0,0]`、标量总推力 `T=mg/cos(phi)`、`t0=0`、`tf=2 s`、`dt=0.01 s`。
- 本人依据工况提示完成参数、动力学闭包及 East 方向解析轨迹，记录为 **H**；从前两个脚本复用的 Codex 结构断言与输出框架保持 **G**，当前脚本整体来源为 H/G 混合。
- 首次实际运行失败：解析式直接使用 `T*sin(phi)` 计算轨迹，遗漏除以质量，混淆了力与加速度；位置断言有 200/603 个元素不匹配，最大绝对差 `1.72917699 m`。经单位提示后，本人补上 `/mass_kg`，等价于使用 `a_e=g*tan(phi)`，该修正保持 **H**。
- 修正后使用固定解释器 `D:\anaconda\python.exe` 和项目 `PYTHONPATH` 实际运行：200 次更新、201 个状态，最终时刻 `2.00 s`，最终六维状态约为 `[0,3.45835397,0,0,3.45835397,0]`，时间与状态全程有限，初始输入未修改。
- 对每个记录时刻使用 `atol=1e-10`、`rtol=0` 比较：最大位置绝对误差为 `9.326e-15 m`，最大速度绝对误差为 `1.332e-14 m/s`，均通过；North/Down 分量保持为零，East 方向与解析关系一致。
- 本次只运行新增正滚转脚本，不是正式 pytest，也未运行周一、周二测试或 Python 全量回归。至此周三任务 B 的三个固定工况均已完成完整轨迹解析验证。

#### 补充概念笔记

- Codex 直接生成 `week_plan/week4/四元数、旋转矩阵、坐标系转换.md`，记录为 **G**。内容展开 NED/FRD、标量 `T` 与向量 `f_T^B`、正滚转轴角四元数、`q_NB→R_NB`、矩阵与 Hamilton 乘积两种向量旋转、世界系力到 `a_N` 及 East 分量 `a_e` 的推导，并记录漏除质量等常见错误。
- 源码结构检查通过：LaTeX 使用成对的 `$$` 块定界符，代码围栏闭合，无旧的 `\[...\]` 块定界符，文件自身 `git diff --check` 无内容错误；`10°` 数值例复算得到 `a_e=1.729176985365 m/s²`、`p_e(2)=v_e(2)=3.458353970729`（单位分别为 m、m/s）。
- 当前只完成 Markdown 与 LaTeX 源码结构核验；实际显示仍取决于用户使用的 Markdown 数学渲染器，尚未记录为预览验收通过。

### 任务 C：可视化与结论

- Codex 按用户要求直接完成 `python/exercises/Week4/Wed/task_C/plot_point_mass_trajectories.py`，绘图脚本、图形布局和三个 PNG 输出均记录为 **G**；不把本阶段直接生成的内容改记为 I/H。
- 三个工况继续共用正式 `simulate_fixed_step` 和 `point_mass_derivative`。绘图前重新构造每个记录时刻的三维解析位置与速度，并分别以 `atol=1e-10`、`rtol=0` 执行完整轨迹断言；图形只展示已经通过的数值验证，不替代误差验收。
- 每个工况生成一张四面板图：N/E/D 数值与解析位置、N/E/D 数值与解析速度、分量位置绝对误差、分量速度绝对误差。标题和副标题标注质量、推力/姿态、步长及时间范围，坐标轴标注单位，数值线与解析线使用不同线型，误差图标出 `1e-10` 容差线和 `1e-18` 显示下限。
- 固定解释器实际运行结果：自由落体为 200 次更新、201 个状态，最大位置误差 `2.132e-14 m`、最大速度误差 `3.908e-14 m/s`；水平悬停为 1000 次更新、1001 个状态，两类最大误差均为 `0.000e+00`；固定正滚转为 200 次更新、201 个状态，最大位置误差 `9.326e-15 m`、最大速度误差 `1.332e-14 m/s`。三组完整轨迹断言均通过。
- 已输出并实际检查 `outputs/free_fall_trajectory.png`、`outputs/hover_trajectory.png` 和 `outputs/positive_roll_trajectory.png`：自由落体的 Down 轨迹、固定正滚转的 East 轨迹分别与解析线重合，悬停六个状态分量保持为零；标题、参数、坐标单位、图例、误差曲线及容差线均可辨认，无明显裁切。
- 本阶段只运行任务 C 绘图与轨迹验证脚本，不是正式 pytest，也未重跑周一、周二测试或 Python 全量回归。周三下一项为任务 D：LeetCode 21。

### 任务 D：新做 21

- 21. 合并两个有序链表：用户报告已经 **AC**，掌握等级记录为 **C（用户自评）**。
- 实际用时和帮助情况未提供；本轮 Codex 未读取或修改题解代码，因此不根据 AC 结果推断其 I/H/G 来源。
- 哑节点用途、尾指针不变量，以及空链表、重复值、剩余链表接入的解释尚未进行验收，保持“未验证”，不因 AC 自动补记为已掌握。
- 本阶段只更新项目日期 `2026-09-02（周三）` 下的进度记录，没有运行代码、测试或前序任务。

## 2026-09-03（周四）滚动记录

### 任务 A：C++ 正式接口与动力学实现

- 新增 `cpp/include/core/point_mass.hpp` 和 `cpp/src/point_mass.cpp`，并将新源文件加入根目录 `CMakeLists.txt` 的 `aerial_core` target；未新增测试或演示 target。
- 本人在 include、声明与 Eigen API 的小步骤提示后，亲自完成头文件、参数/输入结构、六维状态与有限性检查、质量/推力检查、FRD 推力构造、B→N 旋转、NED 加速度及六维导数组装，核心实现记为 **H**。
- 头文件一度误写入函数体；经指出声明与定义的区别后，本人恢复为以分号结束的纯声明。
- 实现保持状态顺序 `[p_n,p_e,p_d,v_n,v_e,v_d]`，FRD 推力为 `[0,0,-T]`，通过现有 `rotateBodyToNed` 得到 NED 推力，并计算 `a_N=f_T^N/m+[0,0,g]`；四元数归一化与非法值拒绝复用现有旋转模块。
- 本人按提示完成 CMake 单行接入，记为 **H**。Codex 随后只直接整理了两个新文件和 CMake 新增行的局部格式，该格式整理与本段进度文档记为 **G**，未改动本人完成的核心逻辑。
- 定向构建命令：从 `aerial-control-lab` 根目录运行 `cmake --build build/windows-mingw-gcc-debug --target aerial_core -j 4`。实际重新配置并编译 `cpp/src/point_mass.cpp`，成功链接 `libaerial_core.a`，结果为 `Built target aerial_core`。
- 本次没有项目源码警告；配置阶段仅有 Eigen 上游已有的 CMake `CMP0146` 弃用提示。两个新文件无尾随空白，CMake 新增行的限定 `git diff --check` 通过；既有 CMake 其他行的历史空白保持未动。
- 本次只构建 `aerial_core`，没有运行 CTest、GoogleTest、pytest 或周三脚本。任务 A 的接口、实现与构建接入已完成；动力学行为和非法输入尚未经 C++ GoogleTest 验证，下一项为周四任务 B 的首个代表性测试。

### 任务 B：主要 GoogleTest

- 本人根据测试目标与 GoogleTest API 提示建立 `cpp/tests/point_mass_test.cpp`，并将其加入 `aerial_core_gtest`；首个代表性测试为 `PointMassDerivativeTest.PositionDerivativeEqualsVelocity`。
- 本人完成六维非零状态、合法质量、零推力、单位姿态、导数调用及六维/位置导数断言的初版，并在命名空间提示后引入 `aerial_control` 中的四个符号，该主体过程记为 **H**。
- 首次定向编译失败：头文件已被正确找到，但 `StateVector`、`PointMassParameters`、`PointMassInput` 和 `pointMassDerivative` 未使用 `aerial_control` 命名空间；补充局部 `using` 后又因匿名命名空间缺少结尾右花括号失败。这两次均保留为定位过程，不记为测试通过。
- 用户明确要求 Codex 直接处理最后局部修正；Codex 直接完成返回值的 `const` 直接初始化、小写命名、将三个断言改为与输入速度分量对应、补齐匿名命名空间结尾及局部格式，这些修改记为 **G**。因此当前首个测试整体来源为 **H/G 混合**，不补记为完整独立实现。
- 定向编译命令：`cmake --build build/windows-mingw-gcc-debug --target aerial_core_gtest -j 4`；实际成功编译 `point_mass_test.cpp` 并链接 `aerial_core_gtest.exe`。
- 定向测试命令：`build/windows-mingw-gcc-debug/aerial_core_gtest.exe --gtest_filter=PointMassDerivativeTest.PositionDerivativeEqualsVelocity`；实际只运行并通过 1 个新测试，结果为 `1 test from 1 test suite`、`1 passed`。
- 本次没有运行其他 GoogleTest、CTest、pytest 或周三脚本；这不是 `aerial_core_gtest` 全量回归结果。下一项为本人编写“零推力只产生 NED 正 Down 重力”的首个物理工况测试。
- 本人随后完成 `PointMassDerivativeTest.ZeroThrustProducesNedGravity`，使用六维零状态、正质量、零标量推力和单位姿态，断言导数加速度分量为 `[0,0,kStandardGravityMps2]`，验证 NED Down 为正；代码由本人根据局部测试要求完成，记为 **H**。
- 定向编译 `aerial_core_gtest` 成功；随后只使用过滤器运行 `PointMassDerivativeTest.ZeroThrustProducesNedGravity`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest 或前序任务；下一项为单位姿态下 `T=mg` 的水平悬停测试。
- 本人完成 `PointMassDerivativeTest.HoverThrustCancelsNedGravity`，最终使用 `m=1.5 kg`、`T=m*kStandardGravityMps2` 和单位姿态，以绝对容差 `1e-12` 检查三个 NED 加速度分量为零，记为 **H**。
- 悬停测试初版使用 `m=1.0 kg` 且使用精确相等断言，会削弱对“推力除以质量”的验证并忽略浮点误差；经提示改为非单位质量与 `EXPECT_NEAR`。一次修正还将测试函数的右花括号误标成命名空间结尾；本人在作用域提示后补齐两层结尾。两个未收口版本均未运行测试。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.HoverThrustCancelsNedGravity`，实际结果为 `1 test from 1 test suite`、`1 passed`。本次没有运行其他 GoogleTest、CTest、pytest 或周三脚本；下一项为固定正滚转的 East 加速度测试。
- 本人完成 `PointMassDerivativeTest.PositiveRollProducesExpectedEastAcceleration`：使用 `m=1.5 kg`、`phi=10°`、`q_NB=[cos(phi/2),sin(phi/2),0,0]` 与 `T=mg/cos(phi)`，以绝对容差 `1e-12` 检查 North/Down 加速度为零、East 加速度为 `g*tan(phi)`，代码在四元数构造、角度与推力补偿的局部提示后由本人完成，记为 **H**。
- 初版测试的 `<cmath>` include 未写完整，并仍使用普通悬停推力 `mg`；经提示修正 include，保持 Eigen 构造函数的 `(w,x,y,z)` 顺序，并将推力改为滚转补偿值。四元数半角与 East 解析值原本正确。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.PositiveRollProducesExpectedEastAcceleration`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest、CTest、pytest 或周三脚本。
- 关于 `a_e=g*tan(phi)`，Codex 已提供从非负标量 `T`、FRD 力向量 `[0,0,-T]`、B→N 旋转、除以质量到 Down 补偿的分步推导；该解释是提示，尚未由本人独立复述验证。
- 用户明确要求 Codex 直接完成当前零质量测试；Codex 新增 `PointMassDerivativeTest.ZeroMassIsRejected`，使用合法六维零状态、零推力、单位姿态，仅将质量设为 `0.0`，并用 `EXPECT_THROW` 检查 `std::invalid_argument`。该测试代码记为 **G**，不能改记为本人代表性非法输入测试。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.ZeroMassIsRejected`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest、CTest、pytest 或周三脚本；下一项由本人完成负推力的代表性非法输入测试。
- 用户随后要求 Codex 直接完成负推力测试，并一度认为只需把推力改为 `0`；Codex 明确指出 `T=0` 是自由落体所需的合法输入，非法测试必须使用 `T<0`，随后直接新增 `PointMassDerivativeTest.NegativeThrustIsRejected`，以 `-1.0 N` 和其余合法输入检查 `std::invalid_argument`。该测试代码记为 **G**。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.NegativeThrustIsRejected`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest、CTest、pytest 或周三脚本；零推力合法与负推力非法的区别已提示，但尚未由本人独立复述验证。
- 用户明确要求 Codex 直接完成状态维度测试；Codex 新增 `PointMassDerivativeTest.InvalidStateDimensionIsRejected`，仅将状态设为五维，质量、零推力与单位姿态均保持合法，并检查抛出 `std::invalid_argument`。该测试代码记为 **G**。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.InvalidStateDimensionIsRejected`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest、CTest、pytest 或周三脚本；这只覆盖一个错误长度，不代表非有限状态或其他维度变体已经验证。
- 用户明确要求 Codex 直接完成输入状态不变性测试；Codex 新增 `PointMassDerivativeTest.DoesNotModifyInputState`，使用六个非零分量、调用前副本和其余合法输入，显式丢弃导数返回值后以零容差 `isApprox` 检查原状态保持不变。该测试代码记为 **G**。
- 定向编译 `aerial_core_gtest` 成功；随后只运行 `PointMassDerivativeTest.DoesNotModifyInputState`，实际结果为 `1 test from 1 test suite`、`1 passed`。没有运行其他 GoogleTest、CTest、pytest 或周三脚本；这不替代非有限状态、四元数边界或全量回归。
- 用户随后明确要求 Codex 直接完成周四任务 B 的剩余部分。Codex 在现有测试文件中新增负/非有限质量、非有限推力、非有限状态、零范数/非有限四元数和非单位四元数归一化测试，并新增复用正式 `rk4Step` 的自由落体、水平悬停、固定正滚转三条完整轨迹测试；这些新增测试、辅助比较函数及局部整理全部记为 **G**。
- 非法输入覆盖现为：质量 `0`、负数、NaN、正负 Inf；推力负数、NaN、正负 Inf；五维状态与六维状态中的 NaN、正负 Inf；零范数和含非有限系数的 `Eigen::Quaterniond`。C++ 接口使用固定类型 `Eigen::Quaterniond`，错误“四元数形状”在编译期由类型约束，不存在与 Python 二维数组相同的运行时形状分支。
- `PointMassDerivativeTest.NonunitQuaternionIsNormalized` 比较同一非平凡正滚转单位四元数及其 3 倍缩放输入的完整六维导数，绝对容差为 `1e-12`，验证动力学正确复用旋转模块的归一化行为。
- 三条正式 RK4 轨迹均从六维零状态和 `t0=0` 开始，以整数步索引构造时刻并检查每个状态全程有限：自由落体使用 `dt=0.01 s`、200 次更新/201 个状态；悬停使用 `dt=0.01 s`、1000 次更新/1001 个状态；正滚转使用 `phi=10°`、`T=mg/cos(phi)`、`dt=0.01 s`、200 次更新/201 个状态。
- 每个记录时刻均逐分量与独立解析状态比较：自由落体使用 `p_d=0.5*g*t^2`、`v_d=g*t`；悬停六维状态为零；正滚转使用 `a_e=g*tan(phi)`、`p_e=0.5*a_e*t^2`、`v_e=a_e*t`。统一采用绝对容差 `1e-10`；当前测试证明所有比较受该阈值约束，但没有单独输出精确最大位置/速度误差，精确误差数值留待后续跨语言整理。
- 定向构建 `cmake --build build/windows-mingw-gcc-debug --target aerial_core_gtest -j 4` 成功，无项目源码警告。限定运行 `aerial_core_gtest.exe --gtest_filter=PointMassDerivativeTest.*`，实际收集并通过 `16 tests from 1 test suite`，总用时约 `27 ms`。
- 本次只运行周四 `PointMassDerivativeTest.*`，没有运行其他 GoogleTest、CTest、pytest 或周三脚本；不能写成 C++ 全量回归。任务 B 的功能与行为覆盖已收口，整体来源为 **H/G 混合**：本人完成的首批结构与三个主要单步物理工况保留 H/H-G 来源，Codex 直接补齐的边界、输入不变性与完整轨迹保持 G；完整轨迹代码不能作为本人独立实现证据。

### 任务 C：新做 141

- 141. 环形链表：用户报告已经 **AC**，掌握等级记录为 **C（用户自评）**。
- 实际用时、获得的帮助及题解代码位置未提供；本轮 Codex 未读取或修改题解代码，因此不根据 AC 推断其 I/H/G 来源。
- 快慢指针必然相遇的原因，以及空链表、单节点无环、单节点自环和普通无环链表四类边界尚未进行解释验收，保持“未验证”；当前 C 级进入后续复习队列。
- 本阶段只更新项目日期 `2026-09-03（周四）` 下的进度记录，没有运行代码或测试。至此周四任务 A/B/C 均已执行，下一阶段按计划进入周五跨语言演示与质量偏差实验。
