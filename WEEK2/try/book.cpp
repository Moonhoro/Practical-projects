#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
void hnder(int num)
{
printf("接收到了%d信号。\n",num);
}
void alarmfunc(int num)
{
printf("接受到了时钟信号%d。\n",num);
alarm(3);
}

int main()
{
 for (int ii=1;ii<=64;ii++)
  signal(ii,hnder);
signal(SIGALRM,alarmfunc);

alarm(3);

while(1)
{
printf("执行了一次任务。\n");
sleep(1);

}

}
