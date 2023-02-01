# 文件介绍
demo01.cpp 为基础的基于tcp的报文传递和接收的客户端程序

demo02.cpp 为基于tcp的报文传递和接收的服务端程序

demo03.cpp 展示tcp传输过程中粘包情况的客户端

demo04.cpp 展示tcp传输过程中粘包情况的服务端

demo05.cpp 利用封装好的函数解决粘包问题的客户端

demo06.cpp 利用封装好的函数解决粘包问题的服务端

demo07.cpp 利用封装好的TcpClient类实现tcp通信的客户端

demo08.cpp 利用封装好的此TcpServer类实现tcp通信的服务端

demo10.cpp 多进程通信的服务端

demo11.cpp 模仿网银APP软件的客户端。

demo12.cpp 模仿网银APP软件的服务端。

demo13.cpp 在demo11.cpp的基础上增加了心跳报文

demo14.cpp 在demo12.cpp的基础上增加了心跳报文

demo31.cpp 利用多进程进行异步通信的客户端

demo32.cpp 异步通信的服务端

demo33.cpp 利用I/O复用实现的异步通信的客户端

fileserver1.cpp 在demo14.cpp的基础上添加了结构体和解析xml的函数 搭建了文件接收的大致框架。

fileserver2.cpp 实现了文件接收功能

fileserver3.cpp 同fileserver2.cpp 无变动仅仅为了对应 tcpputfiles3.cpp

fileserver4.cpp 实现了异步通信 添加了心跳进程

fileserver.cpp 实现了文件的下载

tcpputfiles1.cpp 在demo13.cpp的基础上添加了结构体和解析xml的函数 搭建了文件发送的大致框架。

tcpputfiles2.cpp 实现了文件发生功能

tcpputfiles3.cpp 实现了删除本地原文件或备份本地原文件的功能

tcpputfiles4.cpp 实现了异步通信 添加了心跳进程

tcpputfiles.cpp 同tcpputfiles4.cpp 无变化仅为对应

tcpgetfiles.cpp 基于已有代码修改，实现了文件下载
