# 高级输入输出
## 定位读写
pread()
pwrite()
## 多缓冲区向量读写
malloc(sizeof(row)) 三次
readv()
在数据库中读写的时候，可以提高读写性能。因为分配小块儿内存性能更高，并且也很容易内存对齐。
## 定位+向量读写
preadv()
pwritev()

## 增强版定位+向量读写
对preadv()增加了read/write flag(RWF):
preadv2()
pwritev2()
Flag有:
- RWF_DSYNC 将数据写透到存储设备，相当于文件的write()+fdatasync()
- RWF_SYNC 写透+同时修改文件属性(文件长度，修改时间，访问时间),相当于write()+fsync()
性能会下降，而不是等linux 内核空闲时再写进去。
复习文件打开相关标志open(char *pathname, flag); O_DIRECT 类似于关闭内核的IO

## 快速传输数据
### sendfile
sendfile(int out_fd, int in_fd, offset* offset, size_t count)直接在两个文件描述符之间复制数据，因为复制直接在内核完成而无需再内核空间和用户空间之间传输数据，因此比read(),write()效率要高很多。
在Nginx中非常常用。
in_fd不能是套接字、管道等，必须对应一个支持mmap()操作的文件，out_fd可以用于任意文件。

### splice/tee/vmsplice
这三个系统调用主要用于管道，本质上管道中的数据就在特定的内核缓冲区中(内存中一般是内存页的链表)。
- splice()将缓冲区的内容**移动**到任意文件描述符
- tee()
- vmsplice()从用户空间复制数据到缓冲区(实际上也是指针的复制)

### copy_file_range
与sendfile类似以，但主要用于普通文件之间的数据复制

## 异步和非阻塞读写
异步：是信号驱动的I/O，实际用的较少
阻塞读写: 没数据的时候死等，应用程序以可理解的数据包为单位读写
非阻塞读写: 应用程序需要为每个数据的读写管理读写进度，某些情况下甚至需要使用临时文件等持久存储保存数据

## IO多路复用(IO multiplexing)
### select/pselect
简单来说就是轮询poll。
比如nginx同时有1万个链接，每次轮询读写。
**最早是select,后来是poll,最后有了epoll(增强的poll)**。
文件描述符集合(fd_set)用一个bit表示对应的文件描述符。
- select就很坑，发现nfds需要+1才行。
- pselect可以指定等待事件的信号掩码，来解决phread_sigmask()和select导致的竞态条件。
select还限制1024个
### poll/epoll
poll需要建立非常大的循环，哪个文件描述符上有东西实际上内核已经知道了，但是应用程序还是需要循环，就很繁琐。
### epoll
epoll提供了让内核负责监视多个文件描述符的机制，而用户空间只需要监听单个epoll文件描述的事件即可
- epoll_create创建一个文件描述符，他是假的fd，因此没法sync
- epoll_ctl()
- epoll_wait()
epoll_wait比较重要的是： EPOLLET默认为水平触发(level-trigger), 边沿触发(edge-trigger，类似于电平信号),通常用水平触发。
### select v.s. poll v.s. poll
- select缺点: 可监听的文件描述符最大为1023；返回后未产生事件的文件描述符对应的比特位会被清除，故而需要重新初始化fd_set
- poll解决了select的缺点，但性能相对于epoll差点，因此后来都直接用epoll了

## 作业
几十个客户端连上
