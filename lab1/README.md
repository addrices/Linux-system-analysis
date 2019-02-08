# 实验结果
## 实现的命令
实现的命令有cd，ls，history，history -c，history [numble],!!,!string，'&',管道，重定向

## 文件说明
编译使用make，执行使用make play，管道测试使用的是自己的hello程序(hello.c)和hello2程序(hello2.c),重定向写入文件a中,‘&’测试使用的是sleep程序（sleep.c)
使用make hello来生成所有简单的测试文件

## 管道和重定向：
最多实现3个进程之间的管道通信
./hello | ./hello2 | ./hello2
输出2_2_Hello,world
./hello | ./hello2 
输出2_Hello,world
./hello | ./hello2 | ./hello2 > a
文件a中会出现相应的字符串
管道实现是使用递归的方法，创建2个子进程，生成他们之间的管道，然后child1执行第一个进程，child2递归调用这个过程。

## &：
./sleep&
三秒后输出，但在这期间shell能够正常工作。

# 使用的接口函数
getpwuid函数是通过用户的uid查找用户的passwd数据。

getuid函数返回一个调用程序的真实用户ID。

getcwd函数会将当前工作目录的绝对路径复制到参数buffer所指的内存空间。
    
chdir是C语言中的一个系统调用函数（同cd），用于改变当前工作目录，其参数为Path 目标目录。（cd实现）

opendir()函数的作用是：打开目录句柄。（ls实现）