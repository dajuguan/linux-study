# 多进程并发编程
不要提偶现Bug!
怎么把偶现变成必现，来恢复现场，要提高复现的概率或者分析代码。

## 常见的并发编程模型
- 多路复用(Multiplexer)
    - select, epoll
    - 事件驱动+回调函数
- 多进程
- 多线程
- 多协程
实际上一通百通，最重要的编程模型就是消息队列，其次是共享内存+同步机制。
消息队列(Pipe, Socket, Message)天生是带同步机制的，因此不需要额外的同步机制，但是因为**数据要走内核**，因此性能稍微低一些。

## 从CGI到FastCGI
WebServer的核心特征是能够处理HTTP请求。

CGI: Common Gateway Interface
- 最大的特点是每个请求一个进程。
FastCGI是改进版本(96年提出来的):
- 单个进程可以处理多次请求
- 可以将FastCGI进程从web服务器中独立出去
现在服务器对FastCGI的支持:
FPM(FastCGI Process Manager)就会变得很简单，只需要listen一下就行了
- WebServer直接把数据发给一个空闲的进程，如何实现的？可以通过生产者消费者模型，但是实际上是:
    - 通过listen(), accept()分开的机制，通过在父进程listen,而在子进程中调用accept()
    - 当多个子进程调用accept时，会拿到listen的第一个请求(内核会确保只有一个子进程拿到),accept()会block进程，直到拿到connection。



## References
- https://github.com/lighttpd/spawn-fcgi
用wget/或者curl发一堆请求，看看有多少个进程；或者用ab压测。