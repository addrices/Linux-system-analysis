# Lab2
## 编译过程
使用内核模块实现系统调用。内核模块代码在ptree.c中。先在/ptreemodule中输入:
```{.bash bgcolor = "bg"}
make
gcc -static -o test test.c
```
启动android虚拟机，运行环境:
```{.bash bgcolor = "bg"}
Target : Android 8.0
API : 26
CPU : x86_64
sudo ./emulator @navd -kernel ~/linux-systems-lab02-addrices/kernel/arch/x86/boot/bzImage -show-kernel
```
在/ptreemodule中输入：
```{.bash bgcolor = "bg"}
adb push ptree.ko /data/misc
adb push test /data/misc
adb shell
cd /data/misc
insmod ptree.ko
./test
```
### prinfo数据结构以及变量说明：
|数据结构|说明|
|:---|:---|
|parent_id|父进程的id|
|pid|进程的pid|
|first_child_pid|第一个子进程的id|
|next_sibling_pid|下一个兄弟进程的id|
|state|进程状态|
|uid|进程的uid|
|comm|进程名|
|num|进程的深度|

### 内核中的数据结构以及变量说明：
|内核中数据结构以及变量|对应prinfo中的变量|说明|
|:---|:---|:---|
|real_parent|parent_id|指向父进程的task_struct的指针|
|pid|pid|pid值|
|children|first_child_pid|children是一个list_head|
|sibling|next_sibling_pid|sibling是一个list_head|
|state|state|state值|
|loginuid|uid|loginuid是一个kuid_t值|
|comm|comm|comm是一个字符数组|
**以上内核变量均位于task_struct , /include/sched.h**

list_head是内核链表的结构，代码在/include/list.h,是一个双向链表。父进程的children链表就是子进程们的sibling链表，使用list_entry函数来访问其节点，原理是根据成员在结构体中的偏移，将当前指针减去这个偏移就得到了这个结构体的偏移。

kuid_t的定义也是一个结构体。其中的val值为uid_t类型的。

zygote 进程(有时候为main)在初始化时会启动虚拟机，并加载一些系统资源。这样 zygote fork 出子进程后，子进程也继承了能正常工作的虚拟机和各种系统资源，接下来只需装载 apk 文件的字节码就可以运行应用程序了，可以大大缩短应用的启动时间，所有的应用程序都是由zygote（main）fork出来的,这就是 zygote 进程的主要作用.这应该是为了解决android平台硬件资源不如linux桌面系统，但需要尽快加载应用程序的原因。

本实验中的main函数是由init进程fork出来的，pid号为1737.

部分实验结果：
```{.bash bgcolor = "bg"}
test syscall hello
176 process
swapper/0,0,0,-1,1,-1,0
  init,1,1,0,1170,2,0
    ueventd,1170,1,1,-1,1540,0
    logd,1540,1,1,-1,1541,1036
    servicemanager,1541,1,1,-1,1542,1000
    hwservicemanage,1542,1,1,-1,1543,1000
    vndservicemanag,1543,1,1,-1,1544,1000
    keymaster@3.0-s,1544,1,1,-1,1545,1000
    vold,1545,1,1,-1,1598,0
    allocator@1.0-s,1598,1,1,-1,1599,1000
    audio@2.0-servi,1599,1,1,-1,1601,1041
    provider@2.4-se,1601,1,1,-1,1602,1047
    configstore@1.0,1602,1,1,-1,1603,1000
    drm@1.0-service,1603,1,1,-1,1604,1013
    gatekeeper@1.0-,1604,1,1,-1,1605,1000
    allocator@2.0-s,1605,1,1,-1,1606,1000
    composer@2.1-se,1606,1,1,-1,1607,1000
    power@1.0-servi,1607,1,1,-1,1608,1000
    sensors@1.0-ser,1608,1,1,-1,1609,1000
    wifi@1.0-servic,1609,1,1,-1,1611,1010
    healthd,1611,1,1,-1,1613,0
    qemu-props,1613,1,1,-1,1614,0
    misc-pipe,1614,1,1,-1,1615,0
    lmkd,1615,1,1,-1,1616,0
    surfaceflinger,1616,1,1,-1,1618,1000
    sh,1618,1,1,-1,1737,2000
    main,1737,1,1,1889,1738,0
      system_server,1889,1,1737,-1,2054,1000
      putmethod.latin,2054,1,1737,-1,2079,10054
      m.android.phone,2079,1,1737,-1,2084,1001
      ndroid.systemui,2084,1,1737,-1,2331,10023
      droid.deskclock,2331,1,1737,-1,2483,10037
      .android.dialer,2483,1,1737,-1,2678,10012
      id.printspooler,2678,1,1737,-1,2709,10053
      droid.launcher3,2709,1,1737,-1,2766,10015
      ndroid.keychain,2766,1,1737,-1,2784,1000
      viders.calendar,2784,1,1737,-1,2863,10002
      d.process.acore,2863,1,1737,-1,2913,10000
      ndroid.calendar,2913,1,1737,-1,3018,10032
      droid.messaging,3018,1,1737,-1,3810,10056
      .quicksearchbox,3810,1,1737,-1,-832841812,10051
    main,1738,1,1,-1,1739,0
    audioserver,1739,1,1,-1,1740,1041
    cameraserver,1740,1,1,-1,1741,1047
    drmserver,1741,1,1,-1,1742,1019
    Binder:1742_2,1742,1,1,-1,1743,0
    keystore,1743,1,1,-1,1744,1017
    mediadrmserver,1744,1,1,-1,1745,1013
    mediaextractor,1745,1,1,-1,1746,1040
    mediametrics,1746,1,1,-1,1747,1013
    mediaserver,1747,1,1,-1,1748,1013
    Binder:1748_2,1748,1,1,1761,1749,0
      iptables-restor,1761,1,1748,-1,1766,0
      ip6tables-resto,1766,1,1748,-1,-832841812,0
    storaged,1749,1,1,-1,1750,0
    wificond,1750,1,1,-1,1751,1010
    gnss@1.0-servic,1751,1,1,-1,1752,1000
    omx@1.0-service,1752,1,1,-1,1753,1046
    rild,1753,1,1,-1,1754,1001
    init.ranchu-net,1754,1,1,1777,1755,0
      init.wifi.sh,1777,0,1754,-1,-832841812,0
    gatekeeperd,1755,1,1,-1,1757,1000
    tombstoned,1757,1,1,-1,1758,1058
    fingerprint@2.1,1758,1,1,-1,2173,1000

```
![实验结果截图：](/lab2/实验结果.png)
