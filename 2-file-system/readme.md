# 第二讲 文件及文件系统
## 核心概念

# 作业
## glib中的[文件](https://github.com/GNOME/glib/blob/main/glib/gfileutils.h)和[文件系统接口](https://github.com/GNOME/glib/blob/main/glib/gdir.h)
[如何编译引入Glib的C程序](https://www.cnblogs.com/dgwblog/p/12152816.html)

编译程序:
```
gcc dir.c `pkg-config --cflags --libs glib-2.0`
```

## 改写方同学的程序，使之很容易重现写入混乱；进一步增强上述程序，使之彻底避免写入混乱
```
gcc concurrent_io.c && ./a.out
```