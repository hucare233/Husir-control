
# Funcition

![logo](\md_pictures\logo.png)
![bejite](\md_pictures\bejite.png)
![sanjiu](\md_pictures\sanjiu.png)


---

## Click here

[![hucare: Motor Driver](https://img.shields.io/badge/%E5%8F%AE%E5%92%9A%E8%9B%8B-MotorPro-blue)](http://www.github.com/hucare233)

---

+ tim2:电机超时检测
+ tim3：发送VESC,DJ,Elmo，Epos报文
+ usart1：虚拟示波器
+ usart2：串口屏

## 中断优先级

|中断/事件|抢占优先级|响应优先级|用途|
|:-:|:-:|:-:|:-:|
|TIM3_IRQn|1|0|VESC、DJ电机驱动|
|TIM2_IRQn|3|1|电机超时检测|
|CAN2_RX0_IRQn|2|0|DJ报文获取|
|CAN2_RX1_IRQn|2|1|ELMO/VESC/EPOS报文获取|
|CAN1_RX0_IRQn|3|0|主控|
|USART2_IRQn|4|1|串口屏|
|USART2_DMA|7|2||
|USART1_DMA|6|1|虚拟示波器|
|USART3_IRQn|4|0|编码器|

---  

# 待做的事

+ 将can1的接收放入接收队列（已完成）
+ 转向电机（已完成）
+ 本杰明驱动器的位置模式实现（上位机完成，刘定川接手）
+ 底盘Flat90调试（未完成）

# 注意事项

+ TIM1、TIM8-TIM11的时钟为APB2时钟的两倍即168M，TIM2-TIM7、TIM12~TIM14的时钟为APB1的时钟的两倍即84M。
+ 定时器分频系数不为1，将时钟倍数乘2作为定时器时钟。
+ 编码器线数4倍频
+ 使用操作系统时，在使用与其他任务共享的全局变量要进入临界区
+ `OSTimeDly(1) = 0.1ms`
+ 虚拟示波器用的`CRC16`协议
+ 数码管显示 :

## LED

| Number  |  status   |
| :----: | :-------: |
|  1   | dj超时  |
|  2  | vesc超时 |
| 3  |   elmo超时    |
| 4 |   epos超时   |

## 矩阵键盘

|一|二|三|四|
|:-:|:-:|:-:|:-:|
|F-放歌|E-|D-|C-|
|B-|A-|9-|8-|
|7-STOP|6-BEGIN|5-失能ELMO|4-使能ELMO|
|3-STOP|2-BEGIN|1-失能大疆|0-使能大疆|

---


## ELMO相关事项

+ 电机模式枚举体因ELMO的电流、速度、位置为固定1，2，5
+ enable只是一个状态，用于观测，并不能真正`像3508实现使能
+ 关于ElmoCANopen协议详见`文档9.1.1.1`
+ `intrinsic.PULSE`直接乘4方便计算
+ JV、SP的计算方法  ——    `速度*编码器线数*4/60=jv(sp)`,位置计算同理
+ U10电机有时候会犯病，可能是因为没有霍尔的影响，目前能找到的解决方法只有重新上电，日后可以研究下霍尔的使用
+ PVT模式数组的设置`不可保存`，得在程序中手动添加
+ PVT数组 `QP[1]=0 QV[1]=0;`，MP[3]: 0-不循环 1-循环
+ RM=0在MO=0时发送，PV.PT=1在MO=1之后发送，在下一个BG执行


## EPOS相关事项

+ EPOS的电机使能需要连续在控制字写入`0x06 0x07 0x0F`,使能一次后只需再发送0x0F就行
+ 速度模式王锟学长已经摸得很清楚了，具体可以直接看他总结
+ 位置模式有两种模式，类似于编码器的绝对式和增量式，可以借助代码实践理解
+ 官方提供的halt和quickstop的刹车效果不很理想，平时基本不用，速度模式下的锁位置通过直接更改为位置模式完成
+ 王锟学长之前遇到过4个电机用机构的代码没问题，与主控加入调试后会有驱动器报错现象，初步结论可能是因为主控发的某些报文它也能接收
+ 位置模式再到达目标后，若想再次运动必须`再发一次0x0F`
+ epos没有查询电流的功能，只能查询当前扭矩在除以一个系数算出电流（参数在flat90手册里头）
+ 启动HMM模式，控制字写入`0x1F`，有两个归零速度，一个较快用于寻找机械寻零，一个较慢用来到达索引脉冲
+ HMM模式method设置为`actual method` ，设置当前位置为零点，`offset position 和 homing position均给0`

# 个人臊皮日志——

---

## 2020.10.21

1. 启动长期主控小白培养计划![doge]

---

## 2020.11.24

1. 时隔一个月我又回来啦，代码看的差不太多了，机构目前也没太多事，准备写全向轮了

---

# --to be continue

# 一只19级的咸鱼

[fad]: \md_pictures\fad.png
[shuai]: \md_pictures\shuai.png
[xk]: \md_pictures\xk.png
[cy]: \md_pictures\cy.png
[youl]: \md_pictures\youl.png
[dedede]: \md_pictures\123.gif
[doge]: \md_pictures\doge.png
