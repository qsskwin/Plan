# 8 月 7 日（周五）任务 C：ZYX 欧拉角边界认识

项目级定义已写入 [`docs/coordinate_conventions.md`](../../../../../docs/coordinate_conventions.md) 第 9 节。本任务只固定定义和使用边界，不提前实现欧拉角与四元数的完整转换。

## 1. 本项目的定义

使用右手坐标系和列向量，roll、pitch、yaw 分别记为 $\phi$、$\theta$、$\psi$：

$$
\mathbf{R}_{NB}
=\mathbf{R}_z(\psi)\mathbf{R}_y(\theta)\mathbf{R}_x(\phi).
$$

这称为 ZYX（3-2-1、yaw-pitch-roll）表示。矩阵最右侧最先作用；只写“欧拉角”而不注明 ZYX 和输入/输出坐标系会产生歧义。

## 2. 奇异性

当 $\theta=\pm\pi/2$ 时，$\cos\theta=0$，roll 与 yaw 耦合，无法分别唯一恢复。这是参数化的几何奇异性，即万向节锁，不是增加浮点精度就能消除的问题。

## 3. 使用限制

- 欧拉角主要用于人类阅读、日志和有限范围命令；
- 姿态组合、连续积分和核心坐标变换使用单位四元数或旋转矩阵；
- 同一姿态可能对应多组欧拉角，角度分支也可能产生数值跳变；
- 后续实现转换函数时，必须单独测试 $\theta$ 接近 $\pm\pi/2$ 的情况。
