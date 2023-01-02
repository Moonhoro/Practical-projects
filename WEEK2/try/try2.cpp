#include</root/project/public/_public.h>

CSEM sem;

struct st_pid
{
  int pid;
  char name[51];
};

int main(int argc,char *argv[])
{
//共享内存的标志
  int shmid;
//创建共享内存
  if((shmid=shmget(0x5005,sizeof(struct st_pid),0640|IPC_CREAT))==-1)
  {
  printf("shmget(0x5005) failed\n");
  }
  struct st_pid *stpid=0;
  if((stpid=( struct st_pid *)shmat(shmid,0,0))==(void *)-1){
    printf("shmat failed\n");
    return -1;
  }
//信号量初始化
if(sem.init(0x5005)==false)
{
  printf("信号量初始化失败。");
  return -1;
}
printf("aaa time=%d,val=%d\n",time(0),sem.value());
sem.P();

printf("bbb time=%d,val=%d\n",time(0),sem.value());
printf("pid=%d,name=%s\n",stpid->pid,stpid->name);

stpid->pid=getpid();
strcpy(stpid->name,argv[1]);
sleep(10);
printf("pid=%d,name=%s\n",stpid->pid,stpid->name);

printf("ccc time=%d,val=%d\n",time(0),sem.value());
 sem.V();
printf("ddd time=%d,val=%d\n",time(0),sem.value());
//分离共享内存
 shmdt(stpid);
//删除共享内存
/*if(shmctl(shmid,IPC_RMID,0)==-1)
{
  printf("failed\n");
  return -1;
}
*/
  return 0;
}
