# 第二讲 文件及文件系统
## 核心概念
file descriptior只是个文件，不是文件流，**没有具体的读/写动作**，只有调用read,write的时候才会去读、写。
因此stdin的`STDIN_NO`和pipe出来的两个fd都可以进行读写，stdin是流，但是这个流**可读可写**，不单单是读出来。
`dup2(fd, STDIN_NO)`则只是把STDIN_NO替换为了fd，而不是把stdio的读流替换成了fd的读流，即实际上调用`read(STDIN_NO)`调用的是`read(fd)`,实际上也可以调用`write(STDIN_NO)`，此时则成为`write(STDIN_NO)`。

所以对于`bc`程序内部有`rw`操作，直接dup2就可以了，但是自己的程序需要调用`rw`才能把`sockfd`给读、写出来，这步不可避免。


# 作业
## 1.glib中的[文件](https://github.com/GNOME/glib/blob/main/glib/gfileutils.h)和[文件系统接口](https://github.com/GNOME/glib/blob/main/glib/gdir.h)
[如何编译引入Glib的C程序](https://www.cnblogs.com/dgwblog/p/12152816.html)

利用`glib`的简单demo:
```
gcc glib.c `pkg-config --cflags --libs glib-2.0`
```

## 2.改写方同学的程序，使之很容易重现写入混乱；进一步增强上述程序，使之彻底避免写入混乱
### 问题复现

通过增加每次写入的字符长度，使其超过缓冲区的长度，从而使得问题容易复现
```
gcc concurrent_io.c && ./a.out
```

魏老师还提到使用以下两种方法也可以:
- 无缓冲模式一个字符一个字符地写
- 调用write系统调用，也会很容易出现错乱。

### 使用flock修复上述bug
```
gcc concurrent_io_fix.c && ./a.out
```

## 3.通过伪终端驱动`bc`完成任意精度的四则表达式计算

### C
```
gcc pts.c && ./a.out
```

### python
```
python3 pts.py
```

## 4.给出指定路径下所有文件和目录项的大小总和

### shell
```
chmod +x dir.sh
./dir.sh
```

### C
```
gcc dir.c && ./a.out
```