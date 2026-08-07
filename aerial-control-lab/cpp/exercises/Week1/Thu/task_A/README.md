# 8 月 6 日（周四）任务 A：四元数理论

本文统一 `aerial-control-lab` 的四元数系数顺序、Hamilton 乘法、向量旋转和坐标变换方向。后续 Python、Eigen 和控制算法均以这里的定义为准。

## 1. 系数顺序与基本约定

四元数统一使用标量在前的顺序：

$$
\mathbf{q}=[w,x,y,z]
=
\begin{bmatrix}
w & \mathbf{q}_v^{\mathsf{T}}
\end{bmatrix},
\qquad
\mathbf{q}_v=
\begin{bmatrix}
x&y&z
\end{bmatrix}^{\mathsf{T}}.
$$

- 使用右手坐标系和列向量；
- 四元数乘法使用 Hamilton 约定，记为 $\otimes$；
- $\mathbf{q}_{NB}$ 与 $\mathbf{R}_{NB}$ 方向一致，把机体系 $B$ 中的向量分量转换到 NED 世界系 $N$；
- 代码、文档和测试都不得把 $[w,x,y,z]$ 与 $[x,y,z,w]$ 混用。

## 2. 单位四元数

四元数的范数为

$$
\lVert\mathbf{q}\rVert
=\sqrt{w^2+x^2+y^2+z^2}.
$$

表示纯旋转的四元数必须是单位四元数：

$$
\lVert\mathbf{q}\rVert=1.
$$

任意非零四元数可以归一化为

$$
\widehat{\mathbf{q}}
=\frac{\mathbf{q}}{\lVert\mathbf{q}\rVert}.
$$

零四元数没有方向，不能归一化，也不能表示姿态；实现遇到零四元数时必须明确报错。

## 3. 共轭与逆

四元数的共轭定义为

$$
\mathbf{q}^{*}=[w,-x,-y,-z].
$$

一般非零四元数的逆为

$$
\mathbf{q}^{-1}
=\frac{\mathbf{q}^{*}}{\lVert\mathbf{q}\rVert^2}.
$$

若 $\mathbf{q}$ 是单位四元数，则

$$
\mathbf{q}^{-1}=\mathbf{q}^{*},
\qquad
\mathbf{q}\otimes\mathbf{q}^{*}
=\mathbf{q}^{*}\otimes\mathbf{q}
=[1,0,0,0].
$$

因此，$\mathbf{q}_{NB}$ 的反向旋转为

$$
\mathbf{q}_{BN}=\mathbf{q}_{NB}^{*}.
$$

## 4. Hamilton 乘法

令

$$
\mathbf{p}=[p_w,\mathbf{p}_v],
\qquad
\mathbf{q}=[q_w,\mathbf{q}_v],
$$

则 Hamilton 乘积为

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

展开到 $[w,x,y,z]$ 分量：

$$
\begin{aligned}
w &=p_wq_w-p_xq_x-p_yq_y-p_zq_z,\\
x &=p_wq_x+p_xq_w+p_yq_z-p_zq_y,\\
y &=p_wq_y-p_xq_z+p_yq_w+p_zq_x,\\
z &=p_wq_z+p_xq_y-p_yq_x+p_zq_w.
\end{aligned}
$$

四元数乘法满足结合律，但通常不满足交换律：

$$
(\mathbf{p}\otimes\mathbf{q})\otimes\mathbf{r}
=\mathbf{p}\otimes(\mathbf{q}\otimes\mathbf{r}),
\qquad
\mathbf{p}\otimes\mathbf{q}\ne\mathbf{q}\otimes\mathbf{p}.
$$

## 5. 轴角到四元数

设单位旋转轴为 $\widehat{\mathbf{u}}=[u_x,u_y,u_z]^{\mathsf{T}}$，按右手定则绕该轴旋转 $\theta$，对应单位四元数为

$$
\mathbf{q}(\widehat{\mathbf{u}},\theta)
=
\left[
\cos\frac{\theta}{2},
\;
u_x\sin\frac{\theta}{2},
\;
u_y\sin\frac{\theta}{2},
\;
u_z\sin\frac{\theta}{2}
\right].
$$

这里必须先把旋转轴归一化。半角来自单位四元数对三维旋转的双覆盖关系。

## 6. 使用四元数旋转向量

把三维向量 $\mathbf{v}$ 写成纯四元数

$$
\mathbf{v}_q=[0,\mathbf{v}].
$$

单位四元数 $\mathbf{q}$ 对向量执行主动旋转时：

$$
[0,\mathbf{v}']
=\mathbf{q}\otimes[0,\mathbf{v}]\otimes\mathbf{q}^{*}.
$$

在本项目的坐标变换记号下，$\mathbf{q}_{NB}$ 把机体系分量变换到 NED 世界系：

$$
[0,\mathbf{v}_N]
=\mathbf{q}_{NB}
\otimes[0,\mathbf{v}_B]
\otimes\mathbf{q}_{NB}^{*}.
$$

反向变换为

$$
[0,\mathbf{v}_B]
=\mathbf{q}_{NB}^{*}
\otimes[0,\mathbf{v}_N]
\otimes\mathbf{q}_{NB}.
$$

只有单位四元数才能在上述公式中保持向量模长。正式实现会在旋转前归一化输入四元数，并拒绝零四元数。

## 7. 旋转组合顺序

列向量约定下，先执行 $\mathbf{q}_1$，再执行 $\mathbf{q}_2$，组合四元数为

$$
\mathbf{q}_{\mathrm{total}}
=\mathbf{q}_2\otimes\mathbf{q}_1.
$$

对于连续坐标变换，若 $\mathbf{q}_{BC}$ 把 $C$ 系变换到 $B$ 系，$\mathbf{q}_{NB}$ 把 $B$ 系变换到 $N$ 系，则

$$
\mathbf{q}_{NC}
=\mathbf{q}_{NB}\otimes\mathbf{q}_{BC}.
$$

最右侧的变换最先作用，和旋转矩阵乘法顺序一致。

## 8. 为什么数值运算后需要重新归一化

理论上的单位四元数经过精确乘法或积分后仍应保持单位范数，但浮点舍入、离散积分和传感器噪声会使范数逐步偏离 1。若直接使用漂移后的四元数旋转向量，结果会混入力为缩放的误差。

因此，姿态积分或多次组合后应执行

$$
\mathbf{q}\leftarrow
\frac{\mathbf{q}}{\lVert\mathbf{q}\rVert}.
$$

归一化只能修正范数漂移，不能修正错误的乘法顺序、坐标系方向或角速度符号。

## 9. 为什么 $\mathbf{q}$ 和 $-\mathbf{q}$ 表示同一姿态

用 $-\mathbf{q}$ 旋转向量时：

$$
(-\mathbf{q})\otimes[0,\mathbf{v}]
\otimes(-\mathbf{q})^{*}
=(-1)^2
\mathbf{q}\otimes[0,\mathbf{v}]\otimes\mathbf{q}^{*}.
$$

两个负号相消，因此 $\mathbf{q}$ 和 $-\mathbf{q}$ 得到完全相同的旋转。比较姿态时不应直接要求四个系数逐项相等，而应允许整体符号相反。

## 10. NED/FRD 的 $90^\circ$ 固定例子

绕 NED 的向下轴 $z_N$ 正转 $90^\circ$：

$$
\mathbf{q}_{NB}
=
\left[
\cos\frac{\pi}{4},0,0,\sin\frac{\pi}{4}
\right]
=
\left[
\frac{\sqrt{2}}{2},0,0,\frac{\sqrt{2}}{2}
\right].
$$

它把机体前向单位向量

$$
\mathbf{v}_B=[1,0,0]^{\mathsf{T}}
$$

变换为 NED 世界系的东方单位向量

$$
\mathbf{v}_N=[0,1,0]^{\mathsf{T}}.
$$

该结果必须与前一天的 $\mathbf{R}_z(\pi/2)$ 固定用例一致。

## 11. 实现与验证位置

- 正式 Python 实现：[`python/aerial_control/quaternion.py`](../../../../../python/aerial_control/quaternion.py)
- pytest 测试：[`python/tests/test_quaternion.py`](../../../../../python/tests/test_quaternion.py)
- 项目级约定摘要：[`docs/coordinate_conventions.md`](../../../../../docs/coordinate_conventions.md)

在项目根目录执行：

```bash
PYTHONPATH=python python -m pytest python/tests/test_quaternion.py -v
```

Windows PowerShell 使用：

```powershell
$env:PYTHONPATH = "python"
python -m pytest python/tests/test_quaternion.py -v
```
