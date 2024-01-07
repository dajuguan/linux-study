## 网络编程
netlink 是内核和用户通讯通讯的方式，不仅限于网络，用途很广。
现在一般是使用`libnl`(对netlink的封装)。

## 针对Wifi设备的WPA Supplicant
实现了一个守护进程，专门和kernel打交道，给上层应用提供了一个套接字的链接。

重构成请求、应答的方式。

## Websocket
数据标准化为了TEXT、BINARY。WS做了数据打包的工作，比如数据的切片和重组等。
在网络上编程，lost connection而不是像Unix domain socket那样提示peer died，因此需要一个ping、pong的心跳机制。
WS不单单是应用层的事儿，还做了一些传输层的事儿。
WS有意思的事儿是，链接刚建立的时候，是通过HTTP来握手的, 发送HTTP Upgrade。

dicp client
libnetutils

## 通过man page查看哪些API被废弃
```
man stime
man strtok
```

