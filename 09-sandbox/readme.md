## chroot 系统调用
- 改变进程的根目录为path指定的路径
在ftp服务器中很常用。
存在安全性问题，即使不适用chdir()，也能绕过chroot()的路径。

### 路径解析过程
"." 和".."是在文件系统中真实存在的目录项吗？在文件系统中不一定有，内核中始终表示的含义是一致的。
openat2(cur_dir ,root dir) 可以指定别的root dir。
### 除了文件之外还有哪些系统资源是需要保护的?
用户权限、网络(网络设备接口)、进程号(防止直接给进程发信号)

## namespace
通过命名空间隔离
Cgroup
IPC(posix的共享内存的名字)
Network
Mount
PID
Time
USER
UTS(Hostname, NIS domain name)

## Clone 系统调用
clone系统调用最早的是fork()增强的功能，可以用于创建进程或线程；
后来增加了flag来指定namespace。
thread id(tid)解释: 在不同的线程中，getpid(2)实际上返回的同一个值，即父进程的PID；而gettid()返回的是当前线程对应的内核“进程”的pid，因此会不一样。Linux内核实际上把线程看成的都是进程。

### clone3系统调用
比clone好用点，clone3(struct clone_args *cl_agrs, size_t size)。 
size这个参数做了一种很好的向前兼容的设计方法，万一后面内核参数变多那么就可以换不同的size。
但是Glibc没有提供对clone3()的封装，需要使用syscall()调用该系统调用。

## setns
已经创建了一个子进程，再把他加入名字空间。

## unshare 系统调用
int unshare(int flags)
- UNSHARE_FIELS: 不共享文件描述符
- UNSHARE_IPC:不和父进程共享IPC名字空间，即创建一个新的名字空间

## PID名字空间
kill(2)可以用来发信号的系统调用。
所有ID类的系统资源和系统类似。
在新的PID名字空间中，第一个进程的PID始终为1，该进程相当于该名字空间的init进程。
- 当PID名字空间的init进程终止时，其他所有进程都会收到SIGKILL信号，该namespace会消失
- PID名字空间可嵌套，从Linux3.7开始，PID名字空间嵌套深度最大为32。
为啥level越深没给父进程发信号。

## 其他名字空间(不是ID管理的)
- Mount namespace(mnt)： mount里面的目录是唯一独立可见，没法移动到最外层的namespace。没有嵌套的概念，就是指定一整个挂载点名字。
UTS: Unix time sharing system
Network Information Service (NIS): 比如DNS就是NIS的一类，比如windows中SUMB可以。

## Time namespace
gettimeofday(), time()这些都不太准，现在都是使用clock_gettime(),clock_settime()
时钟类型:
- CLOCK_REALTIME也叫wall_clock(),可设置。用来计算时间可能不准，因为有可能被调了。
- CLOCK_MONOTONIC:系统自引导算起的单调(增加)时钟，但是系统休眠之后就不会增加了。比如ping,pong可以用这个。
- CLOCK_BOOTTIME:与单调时钟一致，但是休眠时间也算
- CLOCK_PROCESS_CPUTIME_ID: 用于度量当前进程所花费的CPU时间
- CLOCK_THREAD_CPPU_ID

因为MONOTONIC和BOOTTIME是每个不一样的，因此需要隔离。

## 权限控制
### 传统权限控制
- 有效(effetive) UID/GID: 有set-uid/gid flag的时，任意用户都可以改owner, group
- set-user/group-ID
### capability 能力
- 传统基于超级用户的权限过于权限和粗暴。
#### 能力的获取和设置
- capset(hdrp,): 一般root设置
- capget():
接口设计上hdrp可以向后兼容。

## 传统资源限制
- getrlimit(): r代表resource
- setrlimit()
可以用来控制:
- RLIMIT_AS: 虚拟内存空间最大值
- RLMIT_CORE: 核文件的最大尺寸。现在默认关闭了，否则直接扔到/dev/null
后来则出来了控制组cgroups
### 控制组(cgroups)
把进程分成层次化的组，然后分别进行不同的限制，比如硬盘、CPU等。
为了便于管理，将一组进程放在一起关联。

## sandbox
比如docker,可以使用:
- unshare(1)：在命令行创建一个新的名字空间
- firejail(1)在沙箱中执行程序
- flatpack(1): 还可以构建、安装并在沙箱环境中运行应用程序。自定义好runtime，解决应用程序的runtime问题。
- bwrap(1): 

## 容器container
容器和沙箱的底层技术相同，但是容器主要是应用打包、部署和迁移。
- Docker
- LXC: linuxcontainer.org (与K8s比较类似)
    - 是linux内核容器功能的一组用户空间接口
    - 各流行linux发行版的容器模版(实际上是一个独立的文件系统，可以增加一些自己的工具、配置)

## 作业
clone3需要syscall
限制比如mount之类
比如busybox，这样可以部署一堆的。