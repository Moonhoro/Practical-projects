#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>



int main()
{
//signal(SIGCHLD,SIG_IGN);
int pid=fork();
if(pid==0){
printf("这是子进程%d,将执行子进程任务\n",getpid());
sleep(5);
}

if(pid>0){
 printf("这是父进程%d,将执行父进程任务。\n",getpid());
//int sts;
//wait(&sts);
sleep(10);
}
}
