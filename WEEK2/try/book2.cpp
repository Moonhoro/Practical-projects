#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
int main()
{
printf("getpid()=%d\n",getpid());
sleep(10);
printf("getppid()=%d\n",getppid());

int pid=fork();
 printf("pid=%d/n",pid);
printf("getpid()=%d\n",getpid());
sleep(50);
printf("getppid()=%d\n",getppid());
}
