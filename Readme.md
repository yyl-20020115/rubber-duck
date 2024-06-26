RubberDuck Copyright (C) 2020 liqun
# 概述
&emsp;&emsp;RubberDuck是一个面向对象的、简单易用的C++仿真库，兼容Windows和Linux操作系统，支持面向事件调度、三段扫描和进程交互仿真策略的离散事件仿真模型开发。初学者基于C++语言可以更快地设计开发离散事件系统仿真模型，另外通过阅读、分析和修改该软件库的源代码和仿真模型示例，也可以更加深入的理解和掌握离散事件系统仿真方法。
如有任何疑问，可以通过邮箱liqun@nudt.edu.cn进行咨询。
# RubberDuck组成
| 目录  | 子目录 | 说明 |
|---|---|---|
|bin||编译生成的RubberDuck静态库libRubberDuck.a和仿真模型示例的可执行程序|
|lib||RubberDuck库源程序|
|examples||仿真模型示例|
||AbleBaker_3P|AbleBaker呼叫中心三段扫描法仿真模型|
||AbleBaker_ES|AbleBaker呼叫中心事件调度法仿真模型|
||AbleBaker_PI|AbleBaker呼叫中心进程交互法仿真模型|
||DumpTruck_3P|卡车卸货问题三段扫描法仿真模型|
||DumpTruck_ES|卡车卸货问题事件调度法仿真模型|
||DumpTruck_PI|卡车卸货问题进程交互法仿真模型|
||Philosopher_3P|哲学家就餐问题三段扫描法仿真模型|
||Philosopher_ES|哲学家就餐问题事件调度法仿真模型|
||Philosopher_PI|哲学家就餐问题进程交互法仿真模型|
||Queue|单通道排队系统手工运行仿真模型|
||QueueBatch|单通道排队系统批量运行仿真模型|
||QueueRandom|单通道排队系统随机仿真模型|
||Queue3P|单通道排队系统三段扫描法仿真模型|
||QueueES|单通道排队系统事件调度法仿真模型|
||QueuePI|单通道排队系统进程交互法仿真模型|
||Random|随机变量生成测试程序|

# 安装
&emsp;&emsp;参见Install.md文件