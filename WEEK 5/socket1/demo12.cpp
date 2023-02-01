/*
 * 程序名：demo12.cpp，此程序用于演示socket通讯的服务端。
 * 作者：moonhoro
*/
#include "../_public.h"
CLogFile logfile;
  CTcpServer TcpServer;

  bool bsession=false;
void FathEXIT(int sig);//父进程退出
void ChldEXIT(int sig);
bool _main(const char *strrecvbuffer,char *strsendbuffer);
bool srv001(const char *strrecvbuffer,char *strsendbuffer);
bool srv002(const char *strrecvbuffer,char *strsendbuffer);
int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    printf("Using:./demo12 port logfile\nExample:./demo12 5005 /root/tmp/demo12.log\n\n"); return -1;
  }

  CloseIOAndSignal();
  signal(SIGINT,FathEXIT);
  signal(SIGTERM,FathEXIT);



if(logfile.Open(argv[2],"a+")==false)
{
  printf("logfile.Open(%s) failed",argv[2]);
  return -1;
}

  if(TcpServer.InitServer(atoi(argv[1]))==false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]);
  }
  while(true)
  {
if(TcpServer.Accept()==false)
{
  logfile.Write("TcpServer.Accept() failed.\n");FathEXIT(-1);
}
logfile.Write("客户端(%s)已链接.\n",TcpServer.GetIP());

if(fork()>0)
{
  TcpServer.CloseClient();
  continue;
}

signal(SIGINT,ChldEXIT);
signal(SIGTERM,ChldEXIT);

TcpServer.CloseListen();
  char strrecvbuffer[1024],strsendbuffer[1024];

  // 第5步：与客户端通讯，接收客户端发过来的报文后，回复ok。
  while (1)
  {
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));
memset(strrecvbuffer,0,sizeof(strsendbuffer));
    if ( (TcpServer.Read(strrecvbuffer))==false) break; // 接收客户端的请求报文。
    logfile.Write("接收：%s\n",strrecvbuffer);
//处理业务主函数。

if(_main(strrecvbuffer,strsendbuffer)==false)break;

    if ( (TcpServer.Write(strsendbuffer))==false) break;// 向客户端发送响应结果。
    logfile.Write("发送：%s\n",strsendbuffer);
  }
ChldEXIT(0);
 }
}
void FathEXIT(int sig)
{
signal(SIGINT,SIG_IGN);
signal(SIGTERM,SIG_IGN);

  logfile.Write("父进程退出.sig=%d\n",sig);

  TcpServer.CloseListen();

  kill(0,15);

  exit(0);
}

void ChldEXIT(int sig)
{
signal(SIGINT,SIG_IGN);
signal(SIGTERM,SIG_IGN);

logfile.Write("子进程退出sig=%d\n",sig);

TcpServer.CloseClient();

exit(0);
}

bool _main(const char *strrecvbuffer,char *strsendbuffer)
{

  int isrvcode=-1;
  GetXMLBuffer(strrecvbuffer,"srvcode",&isrvcode);
if((isrvcode!=1)&&(bsession==false))
{
  strcpy(strsendbuffer,"<retcode>-1</retcode><message>用户未登录</message>");
  return true;
}
  switch(isrvcode)
  {
    case 1:
      srv001(strrecvbuffer,strsendbuffer);break;
    case 2:
      srv002(strrecvbuffer,strsendbuffer);break;
    default:
      logfile.Write("业务代码不合法：%s\n",strrecvbuffer); return false;
  }
  return true;
}

bool srv001(const char *strrecvbuffer,char *strsendbuffer)
{
  char tel[12],password[31];
  GetXMLBuffer(strrecvbuffer,"tel",tel,20);
  GetXMLBuffer(strrecvbuffer,"password",password,30);

  if((strcmp(tel,"1392220000")==0)&&(strcmp(password,"123456")==0))
  {
   strcpy(strsendbuffer,"<retcode>0</retcode><message>成功。</message>");
  bsession=true;
  }
    else 
    strcpy(strsendbuffer,"<retcode>-1</retcode><message>失败。</message>");
return true;
}
                                                                
bool srv002(const char *strrecvbuffer,char *strsendbuffer)     
{                                                              
 char cardid[31];                                   
GetXMLBuffer(strrecvbuffer,"cardid",cardid,30);                    
                                                              
if(strcmp(cardid,"62620000000001")==0)
strcpy(strsendbuffer,"<retcode>0</retcode><ye>108.56</ye>");
else                                                         
strcpy(strsendbuffer,"<retcode>-1</retcode><message>失败。</message>");
return true;
}
