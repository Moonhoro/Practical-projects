#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
int main()
{
printf("getpid()=%d\n",getpid());
printf("getppid()=%d\n",getppid());

sleep(50);
}
