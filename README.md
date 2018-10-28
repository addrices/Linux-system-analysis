# linux-
lab1 实现一个简单的shell

函数使用

getpwuid函数是通过用户的uid查找用户的passwd数据。

getuid函数返回一个调用程序的真实用户ID。

getcwd函数会将当前工作目录的绝对路径复制到参数buffer所指的内存空间。
    
chdir是C语言中的一个系统调用函数（同cd），用于改变当前工作目录，其参数为Path 目标目录。（cd实现）

opendir()函数的作用是：打开目录句柄。（ls实现）