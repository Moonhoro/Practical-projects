#include </root/project/public/_public.h>

#define MAXNUMP_ 1000
#define SHMKEYP_ 0x5095
#define SEMKEYP_ 0x5095
//心跳信息结构体
struct st_pinfo
{
int pid;
char pname[51];
int timeout;
time_t atime; //最后一次心跳时间，用整数表示。
};

class PActive
{
  private:
    CSEM m_sem;
    int m_shmid;
    int m_pos;
    struct st_pinfo *m_shm;
  public:
    PActive();//初始化
    bool AddPInfo(const int timeout,const char *pname);//把当前信息加入共享内存
    
    bool UptATime();//更新共享内存的进程心跳时间

    ~PActive();//删除当前进程的心跳记录

};



int main(int argc,char *argv[])
{
  PActive Active;
  Active.AddPInfo(30,argv[1]);
  while(true)
  {
    Active.UptATime();
  sleep(10);
  }
  Active.~PActive();
/*  if(argc<2){printf("Using:./try3 procname\n");return 0;}
  //创建共享内存
int m_shmid=0;
if((m_shmid=shmget(SHMKEYP_,MAXNUMP_*sizeof(struct st_pinfo),0640|IPC_CREAT))==-1)
{
  printf("shmget(%x) failed\n",MAXNUMP_);return -1;
}

CSEM m_sem;
if((m_sem.init(SEMKEYP_))==false)
{
  printf("m_sem.init(%x) failed",SEMKEYP_);
}

//将共享内存链接
struct st_pinfo *m_shm;
m_shm=(struct st_pinfo *)shmat(m_shmid,0,0);
//创建当前进程的心跳信息，并填入。
struct st_pinfo stpinfo;
memset(&stpinfo,0,sizeof(struct st_pinfo));
stpinfo.pid=getpid();
STRNCPY(stpinfo.pname,sizeof(stpinfo.pname),argv[1],50);
stpinfo.timeout=30;
stpinfo.atime=time(0);
//找个空白位置写入
int m_pos=-1;
//若有一个进程异常推出 但进程信息残留 守护系统可能会误清有相同进程ID的当前程序
 for(int ii=0;ii<MAXNUMP_;ii++)
{
  if(m_shm[ii].pid=stpinfo.pid){m_pos=ii;break;}
}
m_sem.P();
if(m_pos==-1)
for(int ii=0;ii<MAXNUMP_;ii++)
{
  if(m_shm[ii].pid==0)
  {
    m_pos=ii;break;
  }
}
if(m_pos==-1)
{
  m_sem.V();
 printf("共享内存已用完.\n");
 return -1;
}
memcpy(m_shm+m_pos,&stpinfo,sizeof(struct st_pinfo));
m_sem.V();
//更新
while(true)
{
  m_shm[m_pos].atime=time(0);
  sleep(10);
}
//移去
//m_shm[p_pos].pid=0;
memset(m_shm+m_pos,0,sizeof(struct st_pinfo));
  
shmdt(m_shm);

return 0;
*/
}
PActive::PActive()
{
  m_shmid=-1;
  m_pos=-1;
  m_shm=0;
}
bool PActive::AddPInfo(const int timeout,const char *pname)
   {
if(m_pos!=-1)return true;
  //创建共享内存
if((m_shmid=shmget(SHMKEYP_,MAXNUMP_*sizeof(struct st_pinfo),0640|IPC_CREAT))==-1)
{
  printf("shmget(%x) failed\n",MAXNUMP_);return -1;
}

if((m_sem.init(SEMKEYP_))==false)
{
  printf("m_sem.init(%x) failed",SEMKEYP_);
}

//将共享内存链接
m_shm=(struct st_pinfo *)shmat(m_shmid,0,0);
//创建当前进程的心跳信息，并填入。
struct st_pinfo stpinfo;
memset(&stpinfo,0,sizeof(struct st_pinfo));
stpinfo.pid=getpid();
STRNCPY(stpinfo.pname,sizeof(stpinfo.pname),pname,50);
stpinfo.timeout=timeout;
stpinfo.atime=time(0);
//找个空白位置写入
//若有一个进程异常推出 但进程信息残留 守护系统可能会误清有相同进程ID的当前程
 for(int ii=0;ii<MAXNUMP_;ii++){
  if(m_shm[ii].pid=stpinfo.pid){m_pos=ii;break;}}
 m_sem.P();
if(m_pos==-1)
for(int ii=0;ii<MAXNUMP_;ii++)
{
  if(m_shm[ii].pid==0)
  {
    m_pos=ii;break;
  }
}
if(m_pos==-1)
{
  m_sem.V();
 printf("共享内存已用完.\n");
 return -1;
}
memcpy(m_shm+m_pos,&stpinfo,sizeof(struct st_pinfo));
m_sem.V();

return true;
   } 
  bool PActive::UptATime()
  {
  if(m_pos!=-1)return false;
  m_shm[m_pos].atime=time(0);
  return true;
   }

   PActive::~PActive()
   {
if(m_pos!=-1)memset(m_shm+m_pos,0,sizeof(struct st_pinfo));
if(m_shm!=0)shmdt(m_shm);
   }
