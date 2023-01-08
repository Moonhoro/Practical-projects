#include </root/project/public/_public.h>

int main(int argc,char *argv[])
{
//帮助文档
if (argc != 2)
  {
    printf("\n");
    printf("Using:./checkproc logfilename\n");

    printf("Example:/project/tools1/bin/procctl 10 /project/tools1/bin/checkproc /tmp/log/checkproc.log\n\n");

    printf("本程序用于检查后台服务程序是否超时，如果已超时，就终止它。\n");
    printf("注意：\n");
    printf("  1）本程序由procctl启动，运行周期建议为10秒。\n");
    printf("  2）为了避免被普通用户误杀，本程序应该用root用户启动。\n");
    printf("  3）如果要停止本程序，只能用killall -9 终止。\n\n\n");

    return 0;
  }
//忽略全部信号
for(int ii=1;ii<=64;ii++)
{
  CloseIOAndSignal(true);
}
//打开日志文件
CLogFile logfile;
if(logfile.Open(argv[1],"a+")==false)
{
  printf("logfile.Open(%s) failed.\n",argv[1]);
  return -1;
}

//创建共享内存
int shmid=0;
if((shmid=shmget((key_t)SHMKEYP,MAXNUMP*sizeof(struct st_procinfo),0666|IPC_CREAT))==-1)
{
 logfile.Write("创建/获取共享内存（%s）失败",SHMKEYP);
 return false;
}

//链接

struct st_procinfo *shm=(struct st_procinfo *)shmat(shmid,0,0);

//遍历
  for(int ii=0;ii<MAXNUMP;ii++)
  {
  //如果pid==0 continue
  if(shm[ii].pid==0) continue;
  //如果pid!=0 表明是心跳进程记录
  logfile.Write("ii=%d,pid=%d,pname=%s,timeout=%d,atime=%d\n",ii,shm[ii].pid,shm[ii].pname,shm[ii].timeout,shm[ii].atime);
  //发送信号0判断是否存在，如不存在，则删除，continue
  int iret=kill(shm[ii].pid,0);
  if(iret==-1)
  {
    logfile.Write("进程pid=%d(%s)已经不存在。\n",(shm+ii)->pid,(shm+ii)->pname);
    memset(shm+ii,0,sizeof(struct st_procinfo));
    continue;
  }
  //如未超时，continue
  time_t now = time(0);
  if(now-shm[ii].atime<shm[ii].timeout)continue;
  //如超时，发生信号15，尝试正常终结程序
  logfile.Write("进程pid=%d(%s)已经超时\n",(shm+ii)->pid,(shm+ii)->pname);
  
    kill(shm[ii].pid,15);
    //每隔一秒判断一次进程是否存在，累计五秒.
   for(int jj=0;jj<4;jj++)
   {
     sleep(1);
  iret=kill(shm[ii].pid,0);
  if(iret==-1)
    break;
   }
  //如进程仍然存在，发送信号9，

  if(iret==-1)
  {
  logfile.Write("进程pid=%d(%s)已经正常退出\n",(shm+ii)->pid,(shm+ii)->pname);
  }
else
{
  kill(shm[ii].pid,9);
  logfile.Write("进程pid=%d(%s)已经强制退出\n",(shm+ii)->pid,(shm+ii)->pname);
}
  //删除心跳进程
  memset(shm+ii,0,sizeof(struct st_procinfo));
  }
//分离共享内存
shmdt(shm);
return 0;
}
