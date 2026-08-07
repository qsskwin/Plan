# 8 月 5 日（周三）任务 B：旋转矩阵推导

本文闭卷推导三个基本旋转矩阵，明确主动旋转与坐标变换的区别，并验证旋转矩阵的合法性和乘法顺序。

## 1. 统一约定

- 使用右手坐标系；正旋转方向由右手定则确定。
- 使用列向量，旋转矩阵从左侧乘向量。
- 角度统一使用弧度。

旋转矩阵的第 $j$ 列，是第 $j$ 个源坐标轴单位向量经过变换后在目标坐标系中的分量。

## 2. 绕 $x$ 轴旋转

绕 $x$ 轴旋转时，$x$ 分量不变，$y$-$z$ 平面按照右手定则旋转：

$$
\begin{aligned}
\mathbf{e}_x &\longmapsto \mathbf{e}_x, \\
\mathbf{e}_y &\longmapsto \cos\theta\,\mathbf{e}_y+\sin\theta\,\mathbf{e}_z, \\
\mathbf{e}_z &\longmapsto -\sin\theta\,\mathbf{e}_y+\cos\theta\,\mathbf{e}_z.
\end{aligned}
$$

把三个基向量的变换结果依次作为矩阵的列，得到

$$
\mathbf{R}_x(\theta)
=
\begin{bmatrix}
1 & 0 & 0 \\
0 & \cos\theta & -\sin\theta \\
0 & \sin\theta & \cos\theta
\end{bmatrix}.
$$

## 3. 绕 $y$ 轴旋转

绕 $y$ 轴旋转时，$y$ 分量不变，$x$-$z$ 平面按照右手定则旋转：

$$
\begin{aligned}
\mathbf{e}_x &\longmapsto \cos\theta\,\mathbf{e}_x-\sin\theta\,\mathbf{e}_z, \\
\mathbf{e}_y &\longmapsto \mathbf{e}_y, \\
\mathbf{e}_z &\longmapsto \sin\theta\,\mathbf{e}_x+\cos\theta\,\mathbf{e}_z.
\end{aligned}
$$

因此

$$
\mathbf{R}_y(\theta)
=
\begin{bmatrix}
\cos\theta & 0 & \sin\theta \\
0 & 1 & 0 \\
-\sin\theta & 0 & \cos\theta
\end{bmatrix}.
$$

## 4. 绕 $z$ 轴旋转

绕 $z$ 轴旋转时，$z$ 分量不变，$x$-$y$ 平面按照右手定则旋转：

$$
\begin{aligned}
\mathbf{e}_x &\longmapsto \cos\theta\,\mathbf{e}_x+\sin\theta\,\mathbf{e}_y, \\
\mathbf{e}_y &\longmapsto -\sin\theta\,\mathbf{e}_x+\cos\theta\,\mathbf{e}_y, \\
\mathbf{e}_z &\longmapsto \mathbf{e}_z.
\end{aligned}
$$

因此

$$
\mathbf{R}_z(\theta)
=
\begin{bmatrix}
\cos\theta & -\sin\theta & 0 \\
\sin\theta & \cos\theta & 0 \\
0 & 0 & 1
\end{bmatrix}.
$$

## 5. 主动旋转与坐标变换

### 5.1 主动旋转

主动旋转保持坐标系不动，让几何向量本身旋转。若旋转前后的向量都在同一坐标系中表达，则

$$
\mathbf{v}' = \mathbf{R}(\theta)\mathbf{v}.
$$

这里的撇号表示旋转后的新几何向量。

### 5.2 坐标变换

坐标变换保持几何向量不动，只改变描述该向量的坐标系。项目中的

$$
\mathbf{v}_N = \mathbf{R}_{NB}\mathbf{v}_B
$$

表示同一个几何向量分别用机体系 $B$ 和 NED 世界系 $N$ 表达。$\mathbf{R}_{NB}$ 的第一列、第二列和第三列，分别是机体 $x_B$、$y_B$、$z_B$ 轴在 NED 世界系中的分量。

主动旋转与坐标变换可能使用数值相同的矩阵，但二者解释不同。阅读公式时必须先确认“向量是否真的转动”和“输入、输出分别在哪个坐标系中表达”。

## 6. 合法旋转矩阵验证

合法三维旋转矩阵必须满足

$$
\mathbf{R}^{\mathsf{T}}\mathbf{R}=\mathbf{I},
\qquad
\det(\mathbf{R})=+1.
$$

以 $\mathbf{R}_z(\theta)$ 为例：

$$
\begin{aligned}
\mathbf{R}_z^{\mathsf{T}}\mathbf{R}_z
&=
\begin{bmatrix}
\cos\theta & \sin\theta & 0 \\
-\sin\theta & \cos\theta & 0 \\
0 & 0 & 1
\end{bmatrix}
\begin{bmatrix}
\cos\theta & -\sin\theta & 0 \\
\sin\theta & \cos\theta & 0 \\
0 & 0 & 1
\end{bmatrix} \\
&=
\begin{bmatrix}
\cos^2\theta+\sin^2\theta & 0 & 0 \\
0 & \cos^2\theta+\sin^2\theta & 0 \\
0 & 0 & 1
\end{bmatrix} \\
&=\mathbf{I}.
\end{aligned}
$$

同时

$$
\det\!\left(\mathbf{R}_z\right)
=\cos^2\theta+\sin^2\theta
=1.
$$

$\mathbf{R}_x(\theta)$ 和 $\mathbf{R}_y(\theta)$ 可用相同方法验证。正交性说明长度和夹角保持不变，并给出

$$
\mathbf{R}^{-1}=\mathbf{R}^{\mathsf{T}}.
$$

仅满足正交性还不够；行列式为 $-1$ 的矩阵包含镜像反射，不是纯旋转。

## 7. 乘法顺序不可交换

列向量约定下，最右侧的矩阵最先作用。取

$$
\mathbf{e}_x=
\begin{bmatrix}
1&0&0
\end{bmatrix}^{\mathsf{T}},
$$

先绕 $x$ 轴旋转 $90^\circ$，再绕 $z$ 轴旋转 $90^\circ$，得到

$$
\mathbf{R}_z\!\left(\frac{\pi}{2}\right)
\mathbf{R}_x\!\left(\frac{\pi}{2}\right)
\mathbf{e}_x
=\mathbf{e}_y.
$$

交换顺序后得到

$$
\mathbf{R}_x\!\left(\frac{\pi}{2}\right)
\mathbf{R}_z\!\left(\frac{\pi}{2}\right)
\mathbf{e}_x
=\mathbf{e}_z.
$$

两个结果不同，因此一般情况下

$$
\mathbf{R}_z\mathbf{R}_x \ne \mathbf{R}_x\mathbf{R}_z.
$$

## 8. NED/FRD 的 $90^\circ$ 例子

机体相对 NED 世界系绕向下的 $z_N$ 轴正转 $90^\circ$ 时，机体前方由北转向东。此时

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

输入是机体系中的前向单位向量：

$$
\mathbf{v}_B=
\begin{bmatrix}
1&0&0
\end{bmatrix}^{\mathsf{T}}.
$$

输出是该向量在 NED 世界系中的分量：

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

因此输出沿 $y_N$ 正方向，也就是东方。

## 9. Python/NumPy 验证

任务 C 已在 [`rotation.py`](../../../../../python/aerial_control/rotation.py) 中按上述公式实现 `rot_x`、`rot_y` 和 `rot_z`，并在 [`test_rotation.py`](../../../../../python/tests/test_rotation.py) 中验证：

- $0^\circ$、$90^\circ$ 和 $180^\circ$ 的固定结果；
- 正交性与行列式；
- 旋转前后向量模长不变；
- 固定随机种子的组合旋转。

在项目根目录执行：

```bash
PYTHONPATH=python python -m pytest python/tests/test_rotation.py -v
```
