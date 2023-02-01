/*
 * 程序名：fileserver.cpp，此程序用于演示socket通讯的服务端。
 * 作者：moonhoro
*/
#include "/root/project/public/_public.h"

struct st_arg
{
  int  clienttype;          // 客户端类型，1-上传文件；2-下载文件。
  char ip[31];              // 服务端的IP地址。
  int  port;                // 服务端的端口。
  int  ptype;               // 文件上传成功后文件的处理方式：1-删除文件；2-移动到备份目录。
  char clientpath[301];     // 本地文件存放的根目录。
  char clientpathbak[301];  // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
  bool andchild;            // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[301];      // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
  char srvpath[301];        // 服务端文件存放的根目录。
  int  timetvl;             // 扫描本地目录文件的时间间隔，单位：秒。
  int  timeout;             // 进程心跳的超时时间。
  char pname[51];           // 进程名，建议用"tcpputfiles_后缀"的方式。
} starg;

// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer);

void RecvFilesMain();

CLogFile logfile;
  CTcpServer TcpServer;

char strrecvbuffer[1024],strsendbuffer[1024];
void FathEXIT(int sig);//父进程退出
void ChldEXIT(int sig);
bool ClientLogin();
int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    printf("Using:./fileserver port logfile\nExample:./fileserver 5005 /root/tmp/fileserver.log\n\n"); return -1;
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
/*
if(fork()>0)
{
  TcpServer.CloseClient();
  continue;
}

signal(SIGINT,ChldEXIT);
signal(SIGTERM,ChldEXIT);

TcpServer.CloseListen();
*/  

if(ClientLogin()==false)ChldEXIT(-1);

if(starg.clienttype==1)RecvFilesMain();


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

bool ClientLogin()
{
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
  memset(strsendbuffer,0,sizeof(strsendbuffer));

if(TcpServer.Read(strrecvbuffer,20)==false)
{
logfile.Write("TcpServer.Read() failed.\n");return false;
}
logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);

_xmltoarg(strrecvbuffer);

if((starg.clienttype!=1)&&(starg.clienttype!=2))
  strcpy(strsendbuffer,"failed");
  else 
  strcpy(strsendbuffer,"ok");
        
if(TcpServer.Write(strsendbuffer)==false)
{       
logfile.Write("TcpServer.Write() failed.\n");return false;
}       
logfile.Write("%s login %s.\n",TcpServer.GetIP(),strsendbuffer);

  char tel[12],password[31];
  GetXMLBuffer(strrecvbuffer,"tel",tel,20);
  GetXMLBuffer(strrecvbuffer,"password",password,30);

  if((strcmp(tel,"1392220000")==0)&&(strcmp(password,"123456")==0))
  {
   strcpy(strsendbuffer,"<retcode>0</retcode><message>成功。</message>");
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

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));
GetXMLBuffer(strxmlbuffer,"clienttype",&starg.clienttype);
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);
  GetXMLBuffer(strxmlbuffer,"clientpathbak",starg.clientpathbak);
  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);
  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);
  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);
  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if(starg.timetvl>30)starg.timetvl=30;
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
if(starg.timeout<50)starg.timeout=50;

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);
strcat(starg.pname,"_srv");

  return true;
}

void RecvFilesMain()
{
  while(true)
  {
    memset(strsendbuffer,0,sizeof(strsendbuffer));
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));

    if(TcpServer.Read(strrecvbuffer,starg.timetvl+10)==false)
    {
      logfile.Write("TcpServer.Read() failed.\n");
      return;
    }
    logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);

    //处理心跳报文
    if(strcmp(strrecvbuffer,"<activetest>ok</activetest>")==0)
    {
      strcpy(strsendbuffer,"ok");
      logfile.Write("strsendbuffer=%s\n",strsendbuffer);
      if(TcpServer.Write(strsendbuffer)==false)
      {
        logfile.Write("TcpServer.Write failed\n");
        return;
      }
    }
  }
}
