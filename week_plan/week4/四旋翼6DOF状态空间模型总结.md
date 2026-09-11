# 四旋翼无人机 6-DOF 刚体模型与状态空间表示总结

## 1. 6-DOF 到底指什么？

四旋翼作为刚体，在三维空间中具有 **6 个自由度（6 Degrees of Freedom, 6-DOF）**：

### 3 个平移自由度

$$
x,\quad y,\quad z
$$

分别描述无人机质心在三维空间中的位置。

### 3 个转动自由度

$$
\phi,\quad \theta,\quad \psi
$$

分别表示：

- $\phi$：Roll，滚转角
- $\theta$：Pitch，俯仰角
- $\psi$：Yaw，偏航角

因此，从“自由度”或“广义坐标”的角度来看，可以写成

$$
\mathbf q=
[x,y,z,\phi,\theta,\psi]^T
$$

所以：

$$
\boxed{
6\text{-DOF}=3\text{ 个平移自由度}+3\text{ 个转动自由度}
}
$$

---

## 2. 为什么 6-DOF 模型通常有 12 个状态？

关键在于：

$$
\boxed{
\text{自由度数量} \neq \text{状态变量数量}
}
$$

仅知道

$$
x,y,z,\phi,\theta,\psi
$$

只能知道无人机“现在在哪里、朝向哪里”，但不能唯一确定它下一瞬间如何运动。

例如，只知道一维物体的位置

$$
x=0
$$

并不能判断它接下来如何运动，因为它可能：

- 静止：$\dot x=0$
- 向正方向运动：$\dot x>0$
- 向负方向运动：$\dot x<0$

因此，对于完整动力学模型，还必须知道速度。

---

## 3. 为什么建立一阶状态空间模型时要加入速度？

Newton 第二定律本质上是二阶微分方程：

$$
m\ddot x=F
$$

而现代控制理论中的标准状态空间模型通常写成一阶形式：

$$
\boxed{
\dot{\mathbf x}=f(\mathbf x,\mathbf u)
}
$$

为了把二阶微分方程改写成一阶方程，可以定义

$$
x_1=x
$$

$$
x_2=\dot x
$$

则有

$$
\dot x_1=x_2
$$

$$
\dot x_2=\frac{F}{m}
$$

因此，一个二阶自由度通常需要两个一阶状态：

$$
\boxed{
\text{位置}+\text{速度}
}
$$

对于刚体 6-DOF，则通常需要

$$
6\times 2=12
$$

个状态变量。

---

## 4. 四旋翼常见的 12 维状态向量

一种标准定义是

$$
\boxed{
\mathbf x=
[x,y,z,u,v,w,\phi,\theta,\psi,p,q,r]^T
}
$$

其中：

| 状态 | 含义 |
|---|---|
| $x,y,z$ | 无人机的位置 |
| $u,v,w$ | 机体系中的三个线速度分量 |
| $\phi,\theta,\psi$ | Roll、Pitch、Yaw 姿态角 |
| $p,q,r$ | 机体系三个方向的角速度 |

因此可以分成四部分：

$$
\boxed{
\underbrace{x,y,z}_{位置}
+
\underbrace{u,v,w}_{线速度}
+
\underbrace{\phi,\theta,\psi}_{姿态}
+
\underbrace{p,q,r}_{角速度}
}
$$

总共：

$$
3+3+3+3=12
$$

个状态。

---

## 5. 为什么不是直接使用 $\dot x,\dot y,\dot z$？

也可以使用。

例如：

$$
\mathbf x=
[x,y,z,\dot x,\dot y,\dot z,\phi,\theta,\psi,p,q,r]^T
$$

其中

$$
\dot x,\dot y,\dot z
$$

表示惯性坐标系中的速度。

但在飞行器动力学中，更常见的是使用机体系速度

$$
u,v,w
$$

二者之间通过姿态旋转矩阵联系：

$$
\begin{bmatrix}
\dot x\\
\dot y\\
\dot z
\end{bmatrix}
=
R
\begin{bmatrix}
u\\
v\\
w
\end{bmatrix}
$$

因此，两种定义都可以，只要坐标系和符号定义保持一致。

---

## 6. 为什么角速度通常用 $p,q,r$，而不是 $\dot\phi,\dot\theta,\dot\psi$？

这是刚体姿态运动学中的一个重要区别。

机体角速度

$$
[p,q,r]^T
$$

一般 **不等于**

$$
[\dot\phi,\dot\theta,\dot\psi]^T
$$

二者满足

$$
\begin{bmatrix}
\dot\phi\\
\dot\theta\\
\dot\psi
\end{bmatrix}
=
\begin{bmatrix}
1 & \sin\phi\tan\theta & \cos\phi\tan\theta\\
0 & \cos\phi &-\sin\phi\\
0 & \sin\phi/\cos\theta &\cos\phi/\cos\theta
\end{bmatrix}
\begin{bmatrix}
p\\
q\\
r
\end{bmatrix}
$$

所以在完整刚体动力学中，通常选择

$$
p,q,r
$$

作为角速度状态。

---

## 7. “状态变量”到底是什么？

状态变量的核心定义可以理解为：

> 给定当前所有状态变量和当前输入后，应当能够唯一确定系统未来的演化。

也就是说，如果已知

$$
\mathbf x(t)
$$

和输入

$$
\mathbf u(t)
$$

就应该可以通过系统微分方程求出

$$
\dot{\mathbf x}(t)
$$

从而继续预测系统之后的运动。

仅有

$$
x,y,z,\phi,\theta,\psi
$$

无法做到这一点，因为它们不包含无人机当前的线速度和角速度。

因此对于完整刚体动力学，需要同时包含：

$$
\boxed{
\text{构型（位置、姿态）}+\text{速度}
}
$$

---

## 8. 非线性状态空间模型

四旋翼的完整刚体动力学可以统一写成

$$
\boxed{
\dot{\mathbf x}=f(\mathbf x,\mathbf u)
}
$$

这里：

- $\mathbf x$：状态向量
- $\mathbf u$：控制输入
- $f(\cdot)$：由刚体运动学和 Newton-Euler 动力学共同确定的非线性函数

例如可以选择输入为

$$
\mathbf u=
[T,\tau_x,\tau_y,\tau_z]^T
$$

其中：

- $T$：总推力
- $\tau_x$：滚转力矩
- $\tau_y$：俯仰力矩
- $\tau_z$：偏航力矩

---

## 9. 这些方程和现代控制理论是什么关系？

四旋翼动力学本身来源于：

### 平移动力学

$$
m\dot{\mathbf v}=\sum \mathbf F
$$

### 转动动力学

$$
I\dot{\boldsymbol\omega}
+
\boldsymbol\omega\times(I\boldsymbol\omega)
=
\boldsymbol\tau
$$

再加上位置和姿态的运动学方程。

把这些方程整理成

$$
\dot{\mathbf x}=f(\mathbf x,\mathbf u)
$$

以后，就得到了 **非线性状态空间模型**。

因此更准确地说：

$$
\boxed{
\text{Newton-Euler 刚体动力学}
\rightarrow
\text{非线性状态空间模型}
}
$$

状态空间形式是现代控制理论常用的数学表达方式，但动力学方程本身首先来自经典力学。

---

## 10. 如何得到现代控制理论中的 $\dot x=Ax+Bu$？

完整的四旋翼动力学是非线性的：

$$
\dot{\mathbf x}=f(\mathbf x,\mathbf u)
$$

因为其中会出现

$$
\sin\phi,\quad
\cos\theta,\quad
pq,\quad
qr,\quad
\tan\theta
$$

等非线性项。

如果在某个平衡点，例如悬停点

$$
\mathbf x=\mathbf x_0
$$

$$
\mathbf u=\mathbf u_0
$$

附近进行一阶 Taylor 线性化，则有

$$
\delta\dot{\mathbf x}
=
A\delta\mathbf x
+
B\delta\mathbf u
$$

其中

$$
\boxed{
A=
\left.
\frac{\partial f}{\partial \mathbf x}
\right|_{\mathbf x_0,\mathbf u_0}
}
$$

$$
\boxed{
B=
\left.
\frac{\partial f}{\partial \mathbf u}
\right|_{\mathbf x_0,\mathbf u_0}
}
$$

这时就得到现代控制理论中最常见的线性状态空间模型：

$$
\boxed{
\dot{\mathbf x}=A\mathbf x+B\mathbf u
}
$$

进一步还可以写输出方程：

$$
\boxed{
\mathbf y=C\mathbf x+D\mathbf u
}
$$

随后就可以进行：

- 极点配置
- LQR
- Kalman Filter
- 线性 MPC
- 可控性、可观性分析

等。

---

## 11. 什么时候可以只有 6 个状态？

如果只建立一个简化的 **运动学模型**，并直接把线速度或角速度当作输入，则可以只使用

$$
[x,y,z,\phi,\theta,\psi]^T
$$

作为状态。

例如：

$$
\dot x=v_x
$$

$$
\dot y=v_y
$$

$$
\dot z=v_z
$$

甚至把

$$
v_x,v_y,v_z,\dot\phi,\dot\theta,\dot\psi
$$

直接作为控制输入。

这种情况下，确实可以得到 6 状态模型。

但它描述的是：

$$
\boxed{
\text{运动学}
}
$$

而不是完整的：

$$
\boxed{
\text{刚体动力学}
}
$$

因为它没有考虑：

- 质量 $m$
- 转动惯量 $I$
- 推力
- 力矩
- 重力
- 加速度
- 平移和转动之间的动力学耦合

---

## 12. 关键概念总结

最重要的是区分下面几个概念：

### 自由度

$$
\boxed{
6\text{-DOF}=x,y,z,\phi,\theta,\psi
}
$$

表示刚体能够独立进行的 6 种运动。

### 广义坐标

$$
\boxed{
\mathbf q=[x,y,z,\phi,\theta,\psi]^T
}
$$

描述刚体的空间构型。

### 状态变量

对于完整二阶刚体动力学，需要同时知道位置、姿态及其运动速度，因此通常为

$$
\boxed{
\mathbf x=
[x,y,z,u,v,w,\phi,\theta,\psi,p,q,r]^T
}
$$

共 12 个状态。

### 状态空间模型

完整非线性模型：

$$
\boxed{
\dot{\mathbf x}=f(\mathbf x,\mathbf u)
}
$$

悬停点附近线性化后：

$$
\boxed{
\delta\dot{\mathbf x}
=
A\delta\mathbf x+B\delta\mathbf u
}
$$

---

## 13. 一句话理解

可以把整个逻辑记成：

$$
\boxed{
6\text{ 个自由度}
\Rightarrow
6\text{ 个位置/姿态坐标}
}
$$

但完整刚体动力学是二阶系统，所以还需要对应的速度信息：

$$
\boxed{
6\text{ 个广义坐标}
+
6\text{ 个速度状态}
=
12\text{ 个状态}
}
$$

因此：

$$
\boxed{
6\text{-DOF}\neq 6\text{ states}
}
$$

对于采用 Euler 角描述的完整四旋翼刚体模型，通常是：

$$
\boxed{
6\text{-DOF}\quad\Longrightarrow\quad12\text{维状态空间模型}
}
$$
