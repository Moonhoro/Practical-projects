#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
int main()
{
FILE *fp=fopen("/tmp/tmp.txt","w+");
fprintf(fp,"001\n");

int ii=1;
int pid=fork();
if(pid==0){
printf("这是子进程%d,将执行子进程任务\n",getpid());
printf("ii=%d\n",ii++);sleep(1);
printf("ii=%d\n",ii++);sleep(1);
printf("ii=%d\n",ii++);sleep(1);

printf("ii=%d\n",ii++);sleep(1);
printf("ii=%d\n",ii++);sleep(1);
 fprintf(fp,"002\n");

}

if(pid>0){
 printf("这是父进程%d,将执行父进程任务。\n",getpid());
 printf("i=%d\n",ii);sleep(1);
 printf("i=%d\n",ii);sleep(1);
 printf("i=%d\n",ii);sleep(1);
 printf("i=%d\n",ii);sleep(1);
 printf("i=%d\n",ii);sleep(1);
fprintf(fp,"003\n");
}
fclose(fp);
}
