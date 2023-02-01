/*
 * 程序名：tcpputfiles.cpp，此程序用于演示socket通讯的客户端。
 * 作者：moonhoro
 */
#include "/root/project/public/_public.h"

char strrecvbuffer[1024], strsendbuffer[1024];

struct st_arg
{
  int clienttype;          // 客户端类型，1-上传文件；2-下载文件。
  char ip[31];             // 服务端的IP地址。
  int port;                // 服务端的端口。
  int ptype;               // 文件上传成功后本地文件的处理方式：1-删除文件；2-移动到备份目录。
  char clientpath[301];    // 本地文件存放的根目录。
  char clientpathbak[301]; // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
  bool andchild;           // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[301];     // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
  char srvpath[301];       // 服务端文件存放的根目录。
  int timetvl;             // 扫描本地目录文件的时间间隔，单位：秒。
  int timeout;             // 进程心跳的超时时间。
  char pname[51];          // 进程名，建议用"tcpputfiles_后缀"的方式。
} starg;
CLogFile logfile;

void EXIT(int sig);

void _help();

bool AckMessage(const char *strrecvbuffer);

bool SendFile(const int sockfd, const char *filename, const int filesize);
// 上传主函数
bool _tcpputfiles();

bool _xmltoarg(char *strxmlbuffer);

CTcpClient TcpClient;

bool ActiveTest();            // 心跳
bool Login(const char *argv); // 登录业务

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help();
  }
  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("打开日志文件失败（%s）。\n", argv[1]);
    return -1;
  }
  if (_xmltoarg(argv[2]) == false)
    return -1;
  if (TcpClient.ConnectToServer(starg.ip, starg.port) == false)
  {
    logfile.Write("TcpClient.ConnectToServer(%s,%s) failed\n", starg.ip, starg.port);
    EXIT(-1);
  }

  if (Login(argv[2]) == false)
  {
    logfile.Write("Login() failed.\n");
    EXIT(-1);
  }

  while (true)
  {
    if (_tcpputfiles() == false)
    {
      logfile.Write("_tcpputfiles failed\n");
      EXIT(-1);
    }

    sleep(starg.timetvl);
    if (ActiveTest() == false)
      break;
  }

  EXIT(0);
}

bool ActiveTest()
{
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "<activetest>ok</activetest>");
  if (TcpClient.Write(strsendbuffer) == false)
    return false; // 向服务端发送请求报文。
                  // logfile.Write("发送：%s\n", strsendbuffer);

  if ((TcpClient.Read(strrecvbuffer)) == false)
    return false; // 接收服务端的回应报文。
  // logfile.Write("接收：%s\n", strrecvbuffer);

  return true;
}
bool Login(const char *argv)
{
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "%s<clienttype>1</clienttype>", argv);
  if (TcpClient.Write(strsendbuffer) == false)
    return false; // 向服务端发送请求报文。
                  // logfile.Write("发送：%s\n", strsendbuffer);

  if (TcpClient.Read(strrecvbuffer, 20) == false)
    return false; // 接收服务端的回应报文。
                  // logfile.Write("接收：%s\n", strrecvbuffer);

  logfile.Write("登录(%s:%d)成功。\n", starg.ip, starg.port);
  return true;
}
void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n", sig);

  exit(0);
}

void _help()
{
  printf("\n");
  printf("Using:/root/project/tools1/bin/tcpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/root/project/tools1/bin/procctl 20 /root/project/tools1/bin/tcpputfiles /root/log/idc/tcpputfiles_surfdata.log \"<ip>8.130.41.197</ip><port>5005</port><ptype>1</ptype><clientpath>/root/tmp/tcp/surfdata1</clientpath><clientpathbak>/root/tmp/tcp/surfdata1bak</clientpathbak><andchild>true</andchild><matchname>*.XML,*.CSV</matchname><srvpath>/root/tmp/tcp/surfdata2</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>\"\n");
  printf("/root/project/tools1/bin/procctl 20 /root/project/tools1/bin/tcpputfiles /root/log/idc/tcpputfiles_surfdata.log \"<ip>8.130.41.197</ip><port>5005</port><ptype>2</ptype><clientpath>/root/tmp/tcp/surfdata1</clientpath><clientpathbak>/root/tmp/tcp/surfdata1bak</clientpathbak><andchild>true</andchild><matchname>*.XML,*.CSV</matchname><srvpath>/root/tmp/tcp/surfdata2</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>\"\n\n\n");

  printf("本程序是数据中心的公共功能模块，采用tcp协议把文件发送给服务端。\n");
  printf("logfilename   本程序运行的日志文件。\n");
  printf("xmlbuffer     本程序运行的参数，如下：\n");
  printf("ip            服务端的IP地址。\n");
  printf("port          服务端的端口。\n");
  printf("ptype         文件上传成功后的处理方式：1-删除文件；2-移动到备份目录。\n");
  printf("clientpath    本地文件存放的根目录。\n");
  printf("clientpathbak 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。\n");
  printf("andchild      是否上传clientpath目录下各级子目录的文件，true-是；false-否，缺省为false。\n");
  printf("matchname     待上传文件名的匹配规则，如\"*.TXT,*.XML\"\n");
  printf("srvpath       服务端文件存放的根目录。\n");
  printf("timetvl       扫描本地目录文件的时间间隔，单位：秒，取值在1-30之间。\n");
  printf("timeout       本程序的超时时间，单位：秒，视文件大小和网络带宽而定，建议设置50以上。\n");
  printf("pname         进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}

// 把xml解析到参数starg结构
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer, "ip", starg.ip);
  if (strlen(starg.ip) == 0)
  {
    logfile.Write("ip is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "port", &starg.port);
  if (starg.port == 0)
  {
    logfile.Write("port is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  if ((starg.ptype != 1) && (starg.ptype != 2))
  {
    logfile.Write("ptype not in (1,2).\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  if (strlen(starg.clientpath) == 0)
  {
    logfile.Write("clientpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpathbak", starg.clientpathbak);
  if ((starg.ptype == 2) && (strlen(starg.clientpathbak) == 0))
  {
    logfile.Write("clientpathbak is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);

  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  if (strlen(starg.matchname) == 0)
  {
    logfile.Write("matchname is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  if (strlen(starg.srvpath) == 0)
  {
    logfile.Write("srvpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl == 0)
  {
    logfile.Write("timetvl is null.\n");
    return false;
  }

  // 扫描本地目录文件的时间间隔，单位：秒。
  // starg.timetvl没有必要超过30秒。
  if (starg.timetvl > 30)
    starg.timetvl = 30;

  // 进程心跳的超时时间，一定要大于starg.timetvl，没有必要小于50秒。
  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
  if (starg.timeout == 0)
  {
    logfile.Write("timeout is null.\n");
    return false;
  }
  if (starg.timeout < 50)
    starg.timeout = 50;

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
  if (strlen(starg.pname) == 0)
  {
    logfile.Write("pname is null.\n");
    return false;
  }

  return true;
}
bool _tcpputfiles()
{
  // 打开本地目录
  CDir Dir;
  if (Dir.OpenDir(starg.clientpath, starg.matchname, 10000, starg.andchild) == false)
  {
    logfile.Write("Dir.Open(%s) 失败\n", starg.clientpath);
    return false;
  }
  while (true)
  {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
    // 遍历
    if (Dir.ReadDir() == false)
      break;
    // 组成报文
    SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><mtime>%s</mtime><size>%d</size>", Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
    if (TcpClient.Write(strsendbuffer) == false)
    {
      logfile.Write("TcpClient.Write failed.\n");
      return false;
    }

    // 发送文件
    logfile.Write("send %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (SendFile(TcpClient.m_connfd, Dir.m_FullFileName, Dir.m_FileSize) == true)
    {
      logfile.WriteEx("ok.\n");
    }
    else
    {
      logfile.WriteEx("failed.\n");
      TcpClient.Close();
    }
    if (TcpClient.Read(strrecvbuffer, 20) == false)
    {
      logfile.Write("TcpClient.Read failed.\n");
      return false;
    }
    //logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);
    //转存或删除本地文件
    AckMessage(strrecvbuffer);
  }
return true;
}

bool SendFile(const int sockfd, const char *filename, const int filesize)
{
  int onread = 0;     // 每次准备读取的字节数
  int bytes = 0;      // 读取的字节数
  char buffer[1000];  // 存放读取数据
  int totalbytes = 0; // 已读取字节数
  FILE *fp = NULL;

  if ((fp = fopen(filename, "rb")) == NULL)
  {
    return false;
  }
  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    // 计算每次读取的字节数
    if (filesize - totalbytes > 1000)
      onread = 1000;
    else
      onread = filesize - totalbytes;
    // 读取数据
    bytes = fread(buffer, 1, onread, fp);

    if (bytes > 0)
    {
      if (Writen(sockfd, buffer, bytes) == false)
      {
        fclose(fp);
        return false;
      }
    }
    // 计算字节总数
    totalbytes = totalbytes + bytes;

    if (totalbytes == filesize)
    {
      break;
    }
  }
  fclose(fp);

  return true;
}
bool AckMessage(const char *strrecvbuffer)
{
  char filename[301];
  char result[11];
  memset(filename,0,sizeof(filename));
  memset(result,0,sizeof(result));
  GetXMLBuffer(strrecvbuffer,"filename",filename,300);
  GetXMLBuffer(strrecvbuffer,"result",result,10);

  if(strcmp(result,"ok")!=0)return true;

  if(starg.ptype==1)
  {
    if(REMOVE(filename)==false)
    {
      logfile.Write("REMOVE(%s) failed.\n",filename);
      return false;
    }
  }
  if(starg.ptype==2)
  {
    //生成备份目录文件名
    char bakfilename[301];
    STRCPY(bakfilename,sizeof(bakfilename),filename);
    UpdateStr(bakfilename,starg.clientpath,starg.clientpathbak,false);
    if(RENAME(filename,bakfilename)==false)
    {
logfile.Write("RENAME(%s,%s) failed.\n",filename,bakfilename);
return false;
    }
  }
return true;
}
