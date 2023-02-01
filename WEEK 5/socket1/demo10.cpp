/*
 * 程序名：demo10.cpp，此程序用于演示socket通讯的服务端。
 * 作者：moonhoro
*/
#include "../_public.h"
CLogFile logfile;
  CTcpServer TcpServer;

void FathEXIT(int sig);//父进程退出
void ChldEXIT(int sig);

int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    printf("Using:./demo10 port logfile\nExample:./demo10 5005 /root/tmp/demo10.log\n\n"); return -1;
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
  char buffer[102400];

  // 第5步：与客户端通讯，接收客户端发过来的报文后，回复ok。
  while (1)
  {
    memset(buffer,0,sizeof(buffer));
    if ( (TcpServer.Read(buffer))==false) break; // 接收客户端的请求报文。
    logfile.Write("接收：%s\n",buffer);

    strcpy(buffer,"ok");
    if ( (TcpServer.Write(buffer))==false) break;// 向客户端发送响应结果。
    logfile.Write("发送：%s\n",buffer);
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
