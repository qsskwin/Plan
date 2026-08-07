# 坐标系与单位约定

本文档定义 `aerial-control-lab` 使用的坐标系、坐标变换方向以及物理量单位。除非接口另有明确说明，项目中的数学推导、配置、代码和测试均遵循本文约定。

## 1. 基本记号

- 使用右手坐标系和列向量。
- 下标表示向量分量所表达的坐标系。例如，$\mathbf{v}_B$ 表示向量 $\mathbf{v}$ 在机体系 $B$ 中的分量，$\mathbf{v}_N$ 表示同一向量在 NED 世界系 $N$ 中的分量。
- 旋转矩阵和四元数采用“目标坐标系在前、源坐标系在后”的命名方式。
- $\mathbf{R}_{NB}$ 和 $\mathbf{q}_{NB}$ 均表示从机体系 $B$ 到 NED 世界系 $N$ 的坐标变换。

## 2. NED 世界坐标系

世界坐标系 $N$ 使用 NED（North-East-Down）约定：

- $x_N$：指向北（North）；
- $y_N$：指向东（East）；
- $z_N$：指向下（Down）。

该坐标系满足右手定则：

$$
\mathbf{e}_{x_N} \times \mathbf{e}_{y_N} = \mathbf{e}_{z_N}
$$

由于 $z_N$ 正方向向下，重力加速度在 NED 坐标系中写作：

$$
\mathbf{g}_N = \begin{bmatrix} 0 & 0 & +g \end{bmatrix}^{\mathsf{T}}
$$

其中 $g$ 是重力加速度大小，为正标量；标准重力近似为 $9.80665\,\mathrm{m/s^2}$。

## 3. FRD 机体坐标系

机体坐标系 $B$ 使用 FRD（Forward-Right-Down）约定，其原点固定在飞行器上：

- $x_B$：指向机体前方（Forward）；
- $y_B$：指向机体右方（Right）；
- $z_B$：指向机体下方（Down）。

该坐标系同样满足右手定则：

$$
\mathbf{e}_{x_B} \times \mathbf{e}_{y_B} = \mathbf{e}_{z_B}
$$

在 FRD 约定下，四旋翼总推力通常沿机体 $z_B$ 轴的负方向。若推力大小 $T \ge 0$，则机体系中的总推力向量写作：

$$
\mathbf{F}_{T,B} = \begin{bmatrix} 0 & 0 & -T \end{bmatrix}^{\mathsf{T}}
$$

## 4. 机体系与世界系之间的变换

### 4.1 下标方向与反向变换

$\mathbf{R}_{NB}$ 将机体系分量转换为 NED 世界系分量：

$$
\mathbf{v}_N = \mathbf{R}_{NB}\mathbf{v}_B
$$

$\mathbf{q}_{NB}$ 表示与 $\mathbf{R}_{NB}$ 相同方向的姿态变换，也就是把机体系向量转换到 NED 世界系。四元数的系数顺序和乘法细节将在四元数实现阶段继续补充，但其下标方向始终遵循本文定义。

反向变换记为 $\mathbf{R}_{BN}$。旋转矩阵的逆等于其转置，因此：

$$
\mathbf{R}_{BN} = \mathbf{R}_{NB}^{\mathsf{T}}
$$

$$
\mathbf{v}_B
= \mathbf{R}_{BN}\mathbf{v}_N
= \mathbf{R}_{NB}^{\mathsf{T}}\mathbf{v}_N
$$

### 4.2 主动旋转与坐标变换

主动旋转保持坐标系不动，让几何向量本身发生旋转：

$$
\mathbf{v}'=\mathbf{R}(\theta)\mathbf{v}.
$$

坐标变换保持几何向量不动，只改变该向量的坐标表达。例如

$$
\mathbf{v}_N=\mathbf{R}_{NB}\mathbf{v}_B
$$

中的 $\mathbf{v}_N$ 和 $\mathbf{v}_B$ 描述同一个几何向量。本文中的 $\mathbf{R}_{NB}$ 默认按坐标变换解释；若代码或推导使用主动旋转，必须明确写出旋转轴、正方向以及输入和输出所在的坐标系。

### 4.3 $\mathbf{R}_{NB}$ 的列向量含义

记 $(\mathbf{e}_{x_B})_N$ 为机体 $x_B$ 轴的单位向量在 NED 世界系中的分量，其他两轴同理，则

$$
\mathbf{R}_{NB}
=
\begin{bmatrix}
(\mathbf{e}_{x_B})_N &
(\mathbf{e}_{y_B})_N &
(\mathbf{e}_{z_B})_N
\end{bmatrix}.
$$

因此，$\mathbf{R}_{NB}$ 的三列依次表示机体前、右、下三个轴在 NED 世界系中的方向。

## 5. 单位约定

项目统一使用 SI 单位；角度在内部计算、接口和配置中统一使用弧度。

| 物理量 | 单位 | 符号 |
| --- | --- | --- |
| 长度 | 米 | $\mathrm{m}$ |
| 时间 | 秒 | $\mathrm{s}$ |
| 角度 | 弧度 | $\mathrm{rad}$ |
| 角速度 | 弧度每秒 | $\mathrm{rad/s}$ |
| 质量 | 千克 | $\mathrm{kg}$ |
| 力 | 牛顿 | $\mathrm{N}$ |
| 力矩 | 牛顿米 | $\mathrm{N\cdot m}$ |

若日志或界面为了便于阅读而显示角度制，必须显式标注 $\mathrm{deg}$，并在进入计算前转换为弧度。

## 6. 简单例子：机体系与世界系重合

当机体系与 NED 世界系完全重合时：

$$
\mathbf{R}_{NB} = \mathbf{I}
$$

机体前方单位向量在机体系中为：

$$
\mathbf{v}_B = \begin{bmatrix} 1 & 0 & 0 \end{bmatrix}^{\mathsf{T}}
$$

转换到 NED 世界系：

$$
\begin{aligned}
\mathbf{v}_N
&= \mathbf{R}_{NB}\mathbf{v}_B \\
&= \mathbf{I}\begin{bmatrix} 1 & 0 & 0 \end{bmatrix}^{\mathsf{T}} \\
&= \begin{bmatrix} 1 & 0 & 0 \end{bmatrix}^{\mathsf{T}}
\end{aligned}
$$

因此，当两个坐标系重合时，机体前方对应 NED 世界系的北向。

## 7. 绕 NED 向下轴正转 $90^\circ$

机体相对 NED 世界系绕 $z_N$ 轴正转 $90^\circ$。由于 $z_N$ 指向下方，根据右手定则，机体前方由北转向东。对应的机体系到世界系变换为

$$
\mathbf{R}_{NB}
=\mathbf{R}_z\!\left(\frac{\pi}{2}\right)
=
\begin{bmatrix}
0 & -1 & 0 \\
1 & 0 & 0 \\
0 & 0 & 1
\end{bmatrix}.
$$

输入向量是机体系中的前向单位向量：

$$
\mathbf{v}_B=
\begin{bmatrix}
1&0&0
\end{bmatrix}^{\mathsf{T}}.
$$

将其转换到 NED 世界系：

$$
\begin{aligned}
\mathbf{v}_N
&=\mathbf{R}_{NB}\mathbf{v}_B \\
&=
\begin{bmatrix}
0&1&0
\end{bmatrix}^{\mathsf{T}}.
\end{aligned}
$$

输出沿 $y_N$ 正方向，即东方。这里的输入坐标系是机体系 $B$，输出坐标系是 NED 世界系 $N$。

## 8. 四元数约定

### 8.1 系数顺序与乘法

项目中的四元数统一使用标量在前的顺序：

$$
\mathbf{q}=[w,x,y,z].
$$

乘法统一使用 Hamilton 约定，记为 $\otimes$。若

$$
\mathbf{p}=[p_w,\mathbf{p}_v],
\qquad
\mathbf{q}=[q_w,\mathbf{q}_v],
$$

则

$$
\mathbf{p}\otimes\mathbf{q}
=
\left[
p_wq_w-\mathbf{p}_v\cdot\mathbf{q}_v,
\;
p_w\mathbf{q}_v+q_w\mathbf{p}_v
+\mathbf{p}_v\times\mathbf{q}_v
\right].
$$

四元数乘法通常不可交换。列向量约定下，最右侧的旋转最先作用。

### 8.2 $\mathbf{q}_{NB}$ 的方向

$\mathbf{q}_{NB}$ 与 $\mathbf{R}_{NB}$ 表示相同方向的变换。对单位四元数：

$$
[0,\mathbf{v}_N]
=\mathbf{q}_{NB}
\otimes[0,\mathbf{v}_B]
\otimes\mathbf{q}_{NB}^{*}.
$$

反向变换四元数为

$$
\mathbf{q}_{BN}
=\mathbf{q}_{NB}^{-1}
=\mathbf{q}_{NB}^{*}.
$$

若 $\mathbf{q}_{BC}$ 把 $C$ 系向量分量变换到 $B$ 系，则连续变换为

$$
\mathbf{q}_{NC}
=\mathbf{q}_{NB}\otimes\mathbf{q}_{BC}.
$$

### 8.3 归一化与符号等价

表示姿态的四元数必须满足

$$
\lVert\mathbf{q}\rVert=1.
$$

浮点运算或姿态积分后必须重新归一化，避免范数漂移混入向量缩放。零四元数不能归一化，也不能表示姿态。

$\mathbf{q}$ 和 $-\mathbf{q}$ 表示同一个三维姿态，测试和姿态比较必须允许整体符号相反。

完整推导、组合顺序和 $90^\circ$ 固定例子见 [`cpp/exercises/Week1/Thu/task_A/README.md`](../cpp/exercises/Week1/Thu/task_A/README.md)。

## 9. ZYX 欧拉角约定与限制

### 9.1 定义和顺序

项目使用 ZYX（3-2-1、yaw-pitch-roll）欧拉角表示法：

- roll：$\phi$，绕 x 轴；
- pitch：$\theta$，绕 y 轴；
- yaw：$\psi$，绕 z 轴。

对于右手坐标系、列向量和机体系到 NED 世界系的变换，定义为

$$
\mathbf{R}_{NB}(\phi,\theta,\psi)
=\mathbf{R}_z(\psi)\mathbf{R}_y(\theta)\mathbf{R}_x(\phi).
$$

矩阵最右侧最先作用。因此，将该乘积作为主动旋转理解时，向量依次经过 x、y、z 轴旋转；将其作为姿态分解名称时，从左到右称为 ZYX。代码、文档或接口只写“欧拉角”而不写顺序是不完整的。

角度输入统一使用弧度。若日志或界面显示角度制，必须显式标注 `deg` 并在计算前转换。

### 9.2 奇异性与万向节锁

当

$$
\cos\theta=0,
\qquad
\theta=\pm\frac{\pi}{2},
$$

ZYX 表示发生奇异。此时 roll 与 yaw 的作用耦合，无法从姿态中唯一恢复两者，这就是万向节锁。它不是数值精度不足导致的错误，而是欧拉角参数化本身的限制。

即使远离奇异点，同一个姿态也可能对应多组欧拉角；跨越角度分支时还可能出现接近 $2\pi$ 的数值跳变。因此：

- 欧拉角适合人类阅读、日志和有限范围的姿态命令；
- 姿态组合、连续积分和核心坐标变换优先使用单位四元数或旋转矩阵；
- 比较姿态时不得仅逐项比较欧拉角；
- 本周不实现完整欧拉角/四元数互转，后续实现必须单独测试奇异点附近和角度分支。

周五任务的详细说明见 [`cpp/exercises/Week1/Fri/task_C/README.md`](../cpp/exercises/Week1/Fri/task_C/README.md)。
