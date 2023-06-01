/*
 *程序名:xmltodb.cpp 用于把xnl文件入库到MySQL
 *作者：moonhoro
 */

#include "/root/project/public/_public.h"
#include "/root/project/public/db/mysql/_mysql.h"

struct st_arg
{
  char connstr[101];     // 数据库的连接参数。
  char charset[51];      // 数据库的字符集。
  char inifilename[301]; // 数据入库的参数配置文件。
  char xmlpath[301];     // 待入库xml文件存放的目录。
  char xmlpathbak[301];  // xml文件入库后的备份目录。
  char xmlpatherr[301];  // 入库失败的xml文件存放的目录。
  int timetvl;           // 本程序运行的时间间隔，本程序常驻内存。
  int timeout;           // 本程序运行时的超时时间。
  char pname[51];        // 本程序运行时的程序名。
} starg;

connection conn;

CLogFile logfile;

void EXIT(int sig);

void _help();

bool _xmltoarg(char *strxmlbuffer);

CPActive PActive;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help();
    return -1;
  }
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("打开日志文件失败（%s）。\n", argv[1]);
    return -1;
  }
  if (_xmltoarg(argv[2]) == false)
    return -1;

  if (conn.connecttodb(starg.connstr, starg.charset) != 0)
  {
    logfile.Write("connect database(%s) failed.\n%s\n", starg.connstr, conn.m_cda.message);
    return false;
  }
  logfile.Write("connect database(%s) ok.\n", starg.connstr);
}
void EXIT(int sig)
{
  printf("程序退出\n");

  exit(0);
}

void _help(char *argv[])
{
  printf("Using:/project/tools1/bin/xmltodb logfilename xmlbuffer\n\n");

  printf("Sample:/root/project/tools1/bin/procctl 10 /root/project/tools1/bin/xmltodb /root/log/idc/xmltodb.log \"<connstr>127.0.0.1,root,Ggh.20020902,3306</connstr><charset>UTF8</charset><inifilename>/root/project/idc1/ini/xmltodb.xml</inifilename><xmlpath>/root/idcdata/xmltodb/vip1</xmlpath><xmlpathbak>/idcdata/xmltodb/vip1bak</xmlpathbak><xmlpatherr>/root/idcdata/xmltodb/vip2err</xmlpatherr><timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_oracle_vip1</pname>\"\n\n");

  printf("本程序是数据中心的公共功能模块，用于把xml文件入库到Oracle的表中。\n");
  printf("logfilename   本程序运行的日志文件。\n");
  printf("xmlbuffer     本程序运行的参数，用xml表示，具体如下：\n\n");

  printf("connstr     数据库的连接参数，格式：ip,username,password,dbname,port。\n");
  printf("charset     数据库的字符集，这个参数要与数据源数据库保持一致，否则会出现中文乱码的情况。\n");
  printf("inifilename 数据入库的参数配置文件。\n");
  printf("xmlpath     待入库xml文件存放的目录。\n");
  printf("xmlpathbak  xml文件入库后的备份目录。\n");
  printf("xmlpatherr  入库失败的xml文件存放的目录。\n");
  printf("timetvl     本程序的时间间隔，单位：秒，视业务需求而定，2-30之间。\n");
  printf("timeout     本程序的超时时间，单位：秒，视xml文件大小而定，建议设置30以上。\n");
  printf("pname       进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr, 100);
  if (strlen(starg.connstr) == 0)
  {
    logfile.Write("connstr is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "charset", starg.charset, 50);
  if (strlen(starg.charset) == 0)
  {
    logfile.Write("charset is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "inifilename", starg.inifilename, 300);
  if (strlen(starg.inifilename) == 0)
  {
    logfile.Write("inifilename is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "xmlpath", starg.xmlpath, 300);
  if (strlen(starg.xmlpath) == 0)
  {
    logfile.Write("xmlpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "xmlpathbak", starg.xmlpathbak, 300);
  if (strlen(starg.xmlpathbak) == 0)
  {
    logfile.Write("xmlpathbak is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "xmlpatherr", starg.xmlpatherr, 300);
  if (strlen(starg.xmlpatherr) == 0)
  {
    logfile.Write("xmlpatherr is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl < 2)
    starg.timetvl = 2;
  if (starg.timetvl > 30)
    starg.timetvl = 30;

  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
  if (starg.timeout == 0)
  {
    logfile.Write("timeout is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
  if (strlen(starg.pname) == 0)
  {
    logfile.Write("pname is null.\n");
    return false;
  }

  return true;
}
