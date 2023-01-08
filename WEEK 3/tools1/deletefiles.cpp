#include "/root/project/public/_public.h"

// 程序退出和信号2、15的处理函数。
void EXIT(int sig);

int main(int argc,char *argv[])
{
//程序的帮助
if (argc != 4)
  {
    printf("\n");
    printf("Using:/project/tools1/bin/deletefiles pathname matchstr timeout\n\n");

    printf("Example:/root/project/tools1/bin/delrtefiles /root/log/idc \"*.log.20*\" 0.02\n");
    printf("        /root/project/tools1/bin/deletefiles /root/tmp/idc/surfdata \"*.xml,*.json\" 0.01\n");
    printf("        /root/project/tools1/bin/procctl 300 /root/project/tools1/bin/deletefiles /root/log/idc \"*.log.20*\" 0.02\n");
    printf("        /project/tools1/bin/procctl 300 /project/tools1/bin/deletefiles /tmp/idc/surfdata \"*.xml,*.json\" 0.01\n\n");

    printf("这是一个工具程序，用于删除历史的数据文件或日志文件。\n");
    printf("本程序把pathname目录及子目录中timeout天之前的匹配matchstr文件全部删除，timeout可以是小数。\n");
    printf("本程序不写日志文件，也不会在控制台输出任何信息。\n");

    return -1;
  }
//关闭全部信号
//CloseIOAndSignal(true);
signal(SIGINT,EXIT);  signal(SIGTERM,EXIT);

//获取文件超时时间点
char strTimeOut[21];
LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0-(int)(atof(argv[3])*24*60*60));

//打开目录

CDir Dir;
if(Dir.OpenDir(argv[1],argv[2],10000,true)==false)
{
  printf("Dir.OpenDir(%s) failed.\n",argv[1]);
  return -1;
}

char strCmd[1024];
//遍历目录
while(true)
{
if(Dir.ReadDir()==false)break;
if ( strcmp(Dir.m_ModifyTime,strTimeOut)<0)
    {
      if (REMOVE(Dir.m_FullFileName)==0) 
        printf("delete %s failed.\n",Dir.m_FullFileName);
      else
        printf("delete %s ok.\n",Dir.m_FullFileName);
    }
  }


  return 0;
}



void EXIT(int sig)
{
  printf("程序退出，sig=%d\n\n",sig);

  exit(0);
}

