# linux系统分析课程实验
## lab1 实现一个简单的shell

Shell是Linux/UNIX系统下传统的用户和计算机的交互界面。Bash就是一个常见的shell,其可执行文件位置在/bin/bash,是一个普通的Linux应用程序。运行bash即可简单了解其功能,Lab01要求实现一个简化版的shell。

##　lab2 实现一个打印进程树的内核调用
Lab2的任务是对内核进行编程,增加一个新的系统调用ptree,并在Android
设备模拟器和Linux虚拟机上使用该系统调用打印出指定的进程树信息。Lab2要
求尝试Android和Linux平台并熟悉其开发环境。Android和Linux平台可以在许
多不同的架构上运行,但我们将针对的特定平台是X86_64 CPU系列。

## lab3 基于方向的内核同步机制
Lab3 的主要任务是实现一个新的内核同步机制,使得进程可以等待特定的方向事件,当设备被转动到该方向时
自动解锁。