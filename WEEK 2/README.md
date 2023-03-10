# 文件说明
book为Linux信号练习

book1-4为Linux多进程练习

try1为共享内存的练习

try2为信号量练习

tools1内为服务程序调度程序 周期性启动一个程序

# Lnux信号
1. 信号的基本概念
	1. 信号(signal)是软件中断，是进程之间相互传递消息的一种方法，用于通知进程发生了事件，但是，不能给进程传递任何数据。
	2. 信号产生的原因有很多，在Liux下，可以用kill（进程编号）和killall（程序名）命令发送信号。
> 	3. ps -ef lgrep  程序名//查询进程编号 
2.  几个常用的信号名、信号值和默认处理

|信号名 | 信号值 |默认处理动作|发出信号的原因  |
|--------|---------|---------------|-------------------|
|SIGINT|2|A|键盘Ctrl+c中断|
|SIGKILL|9|AEF|kill -9 强制杀死程序|
|SIGSEGV|11|C|无效内存引用|
|SIGALRM|14|A|由alarm(2)发出的信号|
|SIGTERM|15|A|采用"kill"或"killall"通知程序|
|SIGUSR1|10|A|用户自定义信号1|
|SIGUSR2|12|A|用户自定义信号2|
|SIGCHLD|17|B|子进程结束信号|

A 终止进程
B 忽略信号
C终止进程并进行内核映像转储
E无法捕获
F无法忽略

# Linux多进程
## Linux的0、1、2号进程 
1. ide进程：系统创建的第一个进程，加载系统。 
2. systemd进程：系统初始化，是所有其它用户进程的祖先。init 
3. kthreadd进程：负责所有内核线程的调度和管理。
## 进程标识
1. 每个进程都有非负整数的唯一的进程ID
2. `ps -ef lgrep  程序名//查询进程编号`
# fork函数
* fork函数可以创建新进程
* 子进程返回0 父进程返回子进程id
* 子进程获得父进程的数据空间、堆、栈的副本。
* fprintf的内容会先存在缓存区，子进程继承数据空间时也会继承缓存区的内容。
* 父进程子进程执行顺序不确定
* 如果父进程先退出，子进程会成为孤儿进程，子进程的父进程变成1。 
* 如果子进程先退出，内核向父进程发送SIGCHLD信号，如果父进程不处理这个信号，子进程会成为僵尸进程。
	僵尸进程解决方法：
	1. 忽略[[Linux信号#^63fd8e|SIGCHLD信号]]
	2. 在父进程中增加等待子进程的代码（wait()函数）但是父进程无法进行其他工作
	3. 在信号处理函数（signal）中调用wait函数

# Linux共享内存
1. 调用shmget函数获取或创建共享内存
2. 调用shmat把共享内存链接到当前进程地址空间
3. 调用shmdt函数把共享内存从进程中分离
4. 调用shmctl函数删除共享内存（一般不删除）
5. ipcs -m 查看共享内存
## shmget函数
`shmget(key,size_t size,int shmflg)`
key 整数一般为16进制

## shmat函数
`void *shmat(int shmid,const void *shmaddr,int shmf1g);`
后两个一般填0；

# Linux信号量
实现当一个进程正在写入时，另一个进程无法写入

`ipcs -s//查看信号量`

`ipcrm sem smid//信号量ID，删除信号量`

信号量的创建：
1. 获取信号量，有，返回，无，创建。
2. 设置信号量初始值（加入IPC_EXCL标志，确保只有一个进程在创建信号量）

