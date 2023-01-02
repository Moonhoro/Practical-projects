#include<stdio.h>
 #include<stdlib.h>
 #include<string.h>
 #include<unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
int main(int argc,char *argv[]){
if(argc<3)
{
printf("Using:./procctl timetvl program argv ...\n");
printf("Example:/project/tools1/bin/procctl 5 /usr/bin/1s -lt /tmp\n\n");
printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
printf("timetv1行周期，单位：秒。被调度的程序运行结束后，在timetvl秒后会被procctl.重新启动。\n");
printf("program被调度的程序名，必须使用全路径。\n");
printf("argvs 被调度的程序的参数。\n");
printf("注意，本程序不会被ki11杀死，但可以用k11-9强行杀死。\n\n\n");

return 0;
}
//无视linux信号
for(int ii=0;ii<64;ii++)
{
signal(ii,SIG_IGN);
close(ii);
}

if(fork()!=0)exit(0);

signal(SIGCHLD,SIG_DFL);

char *pargv[argc];
for(int ii=2;ii<argc;ii++)
	pargv[ii-2]=argv[ii];
pargv[argc-2]=NULL;



while(true){
	if(fork()==0)
{
//	execl("/usr/bin/ls","/usr/bin/ls","-lt","/tmp",(char*)0);
//	execl(argv[2],argv[2],argv[3],argv[4],(char*)0);
//运行进程
execv(argv[2],pargv);
exit(0);
}
else
{
	//防止僵尸程序
int status;
wait(&status);
sleep(atoi(argv[1]));
}
}
return 0;
}
