/*
 * 程序名：fileserver.cpp，此程序用于演示socket通讯的服务端。
 * 作者：moonhoro
 */
#include "/root/project/public/_public.h"

struct st_arg
{
  int clienttype;          // 客户端类型，1-上传文件；2-下载文件。
  char ip[31];             // 服务端的IP地址。
  int port;                // 服务端的端口。
  int ptype;               // 文件上传成功后文件的处理方式：1-删除文件；2-移动到备份目录。
  char clientpath[301];    // 本地文件存放的根目录。
  char clientpathbak[301]; // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
  bool andchild;           // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[301];     // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
  char srvpath[301];       // 服务端文件存放的根目录。
  char srvpathbak[301];    // 服务器备份文件存放目录
  int timetvl;             // 扫描本地目录文件的时间间隔，单位：秒。
  int timeout;             // 进程心跳的超时时间。
  char pname[51];          // 进程名，建议用"tcpputfiles_后缀"的方式。
} starg;

// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer);

void RecvFilesMain();

void SendFileMain();

CLogFile logfile;
CTcpServer TcpServer;
CPActive PActive;

bool _tcpputfiles();
bool bcontinue = true; // 如果_tcpputfiles发送了文件，bcoontinue为true,初始化为true。
char strrecvbuffer[1024], strsendbuffer[1024];
bool SendFile(const int sockfd, const char *filename, const int filesize);
bool AckMessage(const char *strrecvbuffer);
bool ActiveTest(); // 心跳

void FathEXIT(int sig); // 父进程退出
void ChldEXIT(int sig);
bool ClientLogin();
bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize);
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Using:./fileserver port logfile\nExample:./fileserver 5005 /root/tmp/fileserver.log\n\n");
    return -1;
  }

  CloseIOAndSignal();
  signal(SIGINT, FathEXIT);
  signal(SIGTERM, FathEXIT);

  if (logfile.Open(argv[2], "a+") == false)
  {
    printf("logfile.Open(%s) failed", argv[2]);
    return -1;
  }

  if (TcpServer.InitServer(atoi(argv[1])) == false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n", argv[1]);
  }
  while (true)
  {
    if (TcpServer.Accept() == false)
    {
      logfile.Write("TcpServer.Accept() failed.\n");
      FathEXIT(-1);
    }
    logfile.Write("客户端(%s)已链接.\n", TcpServer.GetIP());

    if (fork() > 0)
    {
      TcpServer.CloseClient();
      continue;
    }

    signal(SIGINT, ChldEXIT);
    signal(SIGTERM, ChldEXIT);

    TcpServer.CloseListen();

    if (ClientLogin() == false)
      ChldEXIT(-1);

    if (starg.clienttype == 1)
      RecvFilesMain();
    if (starg.clienttype == 2)
      SendFileMain();
    ChldEXIT(0);
  }
}
void FathEXIT(int sig)
{
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("父进程退出.sig=%d\n", sig);

  TcpServer.CloseListen();

  kill(0, 15);

  exit(0);
}

void ChldEXIT(int sig)
{
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("子进程退出sig=%d\n", sig);

  TcpServer.CloseClient();

  exit(0);
}

bool ClientLogin()
{
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
  memset(strsendbuffer, 0, sizeof(strsendbuffer));

  if (TcpServer.Read(strrecvbuffer, 20) == false)
  {
    logfile.Write("TcpServer.Read() failed.\n");
    return false;
  }
  // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);

  _xmltoarg(strrecvbuffer);

  if ((starg.clienttype != 1) && (starg.clienttype != 2))
    strcpy(strsendbuffer, "failed");
  else
    strcpy(strsendbuffer, "ok");

  if (TcpServer.Write(strsendbuffer) == false)
  {
    logfile.Write("TcpServer.Write() failed.\n");
    return false;
  }
  logfile.Write("%s login %s.\n", TcpServer.GetIP(), strsendbuffer);

  char tel[12], password[31];
  GetXMLBuffer(strrecvbuffer, "tel", tel, 20);
  GetXMLBuffer(strrecvbuffer, "password", password, 30);

  if ((strcmp(tel, "1392220000") == 0) && (strcmp(password, "123456") == 0))
  {
    strcpy(strsendbuffer, "<retcode>0</retcode><message>成功。</message>");
  }
  else
    strcpy(strsendbuffer, "<retcode>-1</retcode><message>失败。</message>");
  return true;
}

bool srv002(const char *strrecvbuffer, char *strsendbuffer)
{
  char cardid[31];
  GetXMLBuffer(strrecvbuffer, "cardid", cardid, 30);

  if (strcmp(cardid, "62620000000001") == 0)
    strcpy(strsendbuffer, "<retcode>0</retcode><ye>108.56</ye>");
  else
    strcpy(strsendbuffer, "<retcode>-1</retcode><message>失败。</message>");
  return true;
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer, "clienttype", &starg.clienttype);
  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);
  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);
  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl > 30)
    starg.timetvl = 30;
  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
  if (starg.timeout < 50)
    starg.timeout = 50;

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
  strcat(starg.pname, "_srv");

  return true;
}

void RecvFilesMain()
{
  PActive.AddPInfo(starg.timeout, starg.pname);
  while (true)
  {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
    PActive.UptATime();
    if (TcpServer.Read(strrecvbuffer, starg.timetvl + 10) == false)
    {
      logfile.Write("TcpServer.Read() failed.\n");
      return;
    }
    // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);

    // 处理心跳报文
    if (strcmp(strrecvbuffer, "<activetest>ok</activetest>") == 0)
    {
      strcpy(strsendbuffer, "ok");
      // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
      if (TcpServer.Write(strsendbuffer) == false)
      {
        logfile.Write("TcpServer.Write failed\n");
        return;
      }
    }
    if (strncmp(strrecvbuffer, "<filename>", 10) == 0)
    {
      // 解析xml
      char clientfilename[301];
      memset(clientfilename, 0, sizeof(clientfilename));
      char mtime[21];
      memset(mtime, 0, sizeof(mtime));
      int filesize = 0;
      GetXMLBuffer(strrecvbuffer, "filename", clientfilename, 300);
      GetXMLBuffer(strrecvbuffer, "mtime", mtime, 19);
      GetXMLBuffer(strrecvbuffer, "size", &filesize);
      // 生成服务端文件名
      char serverfilename[301];
      memset(serverfilename, 0, sizeof(serverfilename));
      strcpy(serverfilename, clientfilename);
      UpdateStr(serverfilename, starg.clientpath, starg.srvpath, false);
      // 接收内容
      logfile.Write("recv %s(%d) ...", serverfilename, filesize);
      if (RecvFile(TcpServer.m_connfd, serverfilename, mtime, filesize) == true)
      {
        logfile.WriteEx("ok.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>ok</result>", clientfilename);
      }
      else
      {
        logfile.WriteEx("failed.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>failed</result>", clientfilename);
      }

      // 返回
      // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
      if (TcpServer.Write(strsendbuffer) == false)
      {
        logfile.Write("TcpServer.Write failed\n");
        return;
      }
    }
  }
}
bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize)
{
  // 生成临时文件名
  char strfilenametmp[301];
  SNPRINTF(strfilenametmp, sizeof(strfilenametmp), 300, "%s.tmp", filename);

  int totalbytes = 0;
  int onread = 0;
  char buffer[1000];
  FILE *fp = NULL;
  // 创建临时文件
  if ((fp = FOPEN(strfilenametmp, "wb")) == NULL)
    return false;

  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    // 计算接受字节数
    if (filesize - totalbytes > 1000)
      onread = 1000;
    else
      onread = filesize - totalbytes;

    // 接受文件内容
    if (Readn(sockfd, buffer, onread) == false)
    {
      fclose(fp);
      return false;
    }
    // 把接收的内容写入文件
    fwrite(buffer, 1, onread, fp);
    // 计算已接受文件总数
    totalbytes = totalbytes + onread;
    if (totalbytes == filesize)
      break;
  }
  fclose(fp);
  UTime(strfilenametmp, mtime);
  if (RENAME(strfilenametmp, filename) == false)
    return false;
  return true;
}
void SendFileMain()
{
  PActive.AddPInfo(starg.timeout, starg.pname);
  while (true)
  {
    if (_tcpputfiles() == false)
    {
      logfile.Write("_tcpputfiles failed\n");
      return;
    }
    if (bcontinue == false)
    {
      sleep(starg.timetvl);

      if (ActiveTest() == false)
        break;
    }
    PActive.UptATime();
  }
}
bool _tcpputfiles()
{
  // 打开本地目录
  CDir Dir;
  if (Dir.OpenDir(starg.srvpath, starg.matchname, 10000, starg.andchild) == false)
  {
    logfile.Write("Dir.Open(%s) 失败\n", starg.srvpath);
    return false;
  }
  int delayed = 0; // 未收到确认报文的文件数量
  int buflen = 0;  // strrecvbuffer的长度

  bcontinue = false;
  while (true)
  {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
    // 遍历
    if (Dir.ReadDir() == false)
      break;
    bcontinue = true;
    // 组成报文
    SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><mtime>%s</mtime><size>%d</size>", Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
    if (TcpServer.Write(strsendbuffer) == false)
    {
      logfile.Write("TcpServer.Write failed.\n");
      return false;
    }

    // 发送文件
    logfile.Write("send %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (SendFile(TcpServer.m_connfd, Dir.m_FullFileName, Dir.m_FileSize) == true)
    {
      logfile.WriteEx("ok.\n");
      delayed++;
    }
    else
    {
      logfile.WriteEx("failed.\n");
      TcpServer.CloseClient();
    }
    PActive.UptATime();
    while (delayed > 0)
    {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      if (TcpRead(TcpServer.m_connfd, strrecvbuffer, &buflen, -1) == false)
        break;
      delayed--;

      // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);
      //  转存或删除本地文件
      AckMessage(strrecvbuffer);
    }
    while (delayed > 0)
    {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      if (TcpRead(TcpServer.m_connfd, strrecvbuffer, &buflen, 10) == false)
        break;
      delayed--;

      // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);
      //  转存或删除本地文件
      AckMessage(strrecvbuffer);
    }
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
  memset(filename, 0, sizeof(filename));
  memset(result, 0, sizeof(result));
  GetXMLBuffer(strrecvbuffer, "filename", filename, 300);
  GetXMLBuffer(strrecvbuffer, "result", result, 10);
  if (strcmp(result, "ok") != 0)
    return true;

  if (starg.ptype == 1)
  {
    if (REMOVE(filename) == false)
    {
      logfile.Write("REMOVE(%s) failed.\n", filename);
      return false;
    }
  }
  if (starg.ptype == 2)
  {
    // 生成备份目录文件名
    char bakfilename[301];
    STRCPY(bakfilename, sizeof(bakfilename), filename);
    UpdateStr(bakfilename, starg.srvpath, starg.srvpathbak, false);
    if (RENAME(filename, bakfilename) == false)
    {
      logfile.Write("RENAME(%s,%s) failed.\n", filename, bakfilename);
      return false;
    }
  }
  return true;
}
bool ActiveTest()
{
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "<activetest>ok</activetest>");
  if (TcpServer.Write(strsendbuffer) == false)
    return false; // 向服务端发送请求报文。
                  // logfile.Write("发送：%s\n", strsendbuffer);

  if ((TcpServer.Read(strrecvbuffer)) == false)
    return false; // 接收服务端的回应报文。
  // logfile.Write("接收：%s\n", strrecvbuffer);

  return true;
}