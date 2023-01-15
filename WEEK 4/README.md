# 文件介绍
try/ftpclicent.cpp 为熟悉ftp基本命令的尝试文件

ftpgetfiles系列为从服务器上下载文件的程序

ftpgetfiles1.cpp 完成了程序的基本框架 写了帮助文档以及xml形式输入的解析

ftpgetfiles2.cpp 完成了登录服务器 获取服务器的文件目标目录的文件 并将其储存在本地 并且加载到容器vlistfile中

ftpgetfiles3.cpp 增加了 删除 转存 两种模式

ftpgetfiles.cpp 最终成品 增加了获取修改文件新增文件的模式 增加了心跳进程

ftpputfiles.cpp 为从本地上传文件到服务器基于ftpgetfiles.cpp改造

# 一些重点
ftpgetfiles中的获取新增模式 vlistfile1中储存已获取文件的名字和时间 vlistfile2中为目标目录下的所有文件的名字和时间 将相同的储存在vlistfile3中 不同的储存在vlistfile4中 在完成之后将vlistfile2与vlistfile4内容互换 可以不改变之前代码 减小工作量

#遇到的问题以及解决
1. ftp安装以及链接问题
安装正常，但是链接异常，因为我使用的是云服务器，除了打开服务器防火墙还需要打开云服务器后台的端口.
2. 库文件不存在
照做百度一顿操作
3.segmentation fault 
找了一下午bug 发现是函数格式没打对
