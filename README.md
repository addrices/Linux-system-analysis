# Linux 系统分析课程实验

## lab1 实现一个简单的 shell

Shell 是 Linux/UNIX 系统下传统的用户和计算机的交互界面。Bash 就是一个常见的 shell,其可执行文件位置在/bin/bash,是一个普通的 Linux 应用程序。运行 bash 即可简单了解其功能,Lab01 要求实现一个简化版的 shell。

## lab2 实现一个打印进程树的内核调用

Lab2 的任务是对内核进行编程,增加一个新的系统调用 ptree,并在 Android
设备模拟器和 Linux 虚拟机上使用该系统调用打印出指定的进程树信息。Lab2 要
求尝试 Android 和 Linux 平台并熟悉其开发环境。Android 和 Linux 平台可以在许
多不同的架构上运行,但我们将针对的特定平台是 X86_64 CPU 系列。

## lab3 基于方向的内核同步机制

Lab3 的主要任务是实现一个新的内核同步机制,使得进程可以等待特定的方向事件,当设备被转动到该方向时
自动解锁。
