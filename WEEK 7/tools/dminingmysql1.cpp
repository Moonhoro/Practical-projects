/*
 *程序名:dminingmysql.cpp 用于从myql数据库中抽取数据，生成xml文件(完成基本框架)
 *作者：moonhoro
 */

#include "/root/project/public/_public.h"
#include "/root/project/public/db/mysql/_mysql.h"

struct st_arg
{
  char connstr[101];     // 数据库的连接参数。
  char charset[51];      // 数据库的字符集。
  char selectsql[1024];  // 从数据源数据库抽取数据的SQL语句。
  char fieldstr[501];    // 抽取数据的SQL语句输出结果集字段名，字段名之间用逗号分隔。
  char fieldlen[501];    // 抽取数据的SQL语句输出结果集字段的长度，用逗号分隔。
  char bfilename[31];    // 输出xml文件的前缀。
  char efilename[31];    // 输出xml文件的后缀。
  char outpath[301];     // 输出xml文件存放的目录。
  int maxcount;          // 输出xml文件最大记录数，0表示无限制。
  char starttime[52];    // 程序运行的时间区间
  char incfield[31];     // 递增字段名。
  char incfilename[301]; // 已抽取数据的递增字段最大值存放的文件。
  char connstr1[101];    // 已抽取数据的递增字段最大值存放的数据库的连接参数。
  int timeout;           // 进程心跳的超时时间。
  char pname[51];        // 进程名，建议用"dminingmysql_后缀"的方式。
} starg;

CLogFile logfile;

void EXIT(int sig);

void _help();

bool _xmltoarg(char *strxmlbuffer);

bool _dminingmysql();

bool LoadOKFile();

bool CompVector();

bool WriteToOKFile();

bool AppendToOKFile(struct st_fileinfo *stfileinfo);

CPActive PActive;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help();
    return -1;
  }

  // 关闭信号
  //  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);
  // 打开日志文件

  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("logfile.Open failed.");
    return -1;
  }
  // 解析xml
  if (_xmltoarg(argv[2]) == false)
    return -1;

  // 写入心跳信息
  PActive.AddPInfo(starg.timeout, starg.pname);
  // 登录服务器

  // logfile.Write("登录(%s,%s,%s)成功\n", starg.host, starg.username, starg.password);

  _dminingmysql();

  return 0;
}

bool _dminingmysql()
{
  return true;
}

void EXIT(int sig)
{
  printf("程序退出\n");

  exit(0);
}

void _help()
{
  printf("Using:/root/project/tools1/bin/dminingmysql logfilename xmlbuffer\n\n");

  printf("Sample:/root/project/tools1/bin/procctl 3600 /root/project/tools1/bin/dminingmysql /root/log/idc/dminingmysql_ZHOBTCODE.log \"<connstr>127.0.0.1,root,Ggh.20020902,mysql,3306</connstr><charset>utf8</charset><selectsql>select obtid,cityname,provname,lat,lon,height from T_ZHOBTCODE</selectsql><fieldstr>obtid,cityname,provname,lat,lon,height</fieldstr><fieldlen>10,30,30,10,10,10</fieldlen><bfilename>ZHOBTCODE</bfilename><efilename>HYCZ</efilename><outpath>/root/idcdata/dmindata</outpath><timeout>30</timeout><pname>dminingmysql_ZHOBTCODE</pname>\"\n\n");
  printf("       /root/project/tools1/bin/procctl   30 /root/project/tools1/bin/dminingmysql /root/log/idc/dminingmysql_ZHOBTMIND.log \"<connstr>127.0.0.1,root,mysqlpwd,mysql,3306</connstr><charset>utf8</charset><selectsql>select obtid,date_format(ddatetime,'%%%%Y-%%%%m-%%%%d %%%%H:%%%%i:%%%%s'),t,p,u,wd,wf,r,vis,keyid from T_ZHOBTMIND where keyid>:1 and ddatetime>timestampadd(minute,-120,now())</selectsql><fieldstr>obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>10,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>ZHOBTMIND</bfilename><efilename>HYCZ</efilename><outpath>/root/idcdata/dmindata</outpath><starttime></starttime><incfield>keyid</incfield><incfilename>/root/idcdata/dmining/dminingmysql_ZHOBTMIND_HYCZ.list</incfilename><timeout>30</timeout><pname>dminingmysql_ZHOBTMIND_HYCZ</pname><maxcount>1000</maxcount><connstr1>127.0.0.1,root,mysqlpwd,mysql,3306</connstr1>\"\n\n");

  printf("本程序是数据中心的公共功能模块，用于从MySQL数据库源表抽取数据，生成xml文件。\n");
  printf("logfilename 本程序运行的日志文件。\n");
  printf("xmlbuffer   本程序运行的参数，用xml表示，具体如下：\n\n");

  printf("connstr     数据库的连接参数，格式：ip,username,password,dbname,port。\n");
  printf("charset     数据库的字符集，这个参数要与数据源数据库保持一致，否则会出现中文乱码的情况。\n");
  printf("selectsql   从数据源数据库抽取数据的SQL语句，注意：时间函数的百分号%需要四个，显示出来才有两个，被prepare之后将剩一个。\n");
  printf("fieldstr    抽取数据的SQL语句输出结果集字段名，中间用逗号分隔，将作为xml文件的字段名。\n");
  printf("fieldlen    抽取数据的SQL语句输出结果集字段的长度，中间用逗号分隔。fieldstr与fieldlen的字段必须一一对应。\n");
  printf("bfilename   输出xml文件的前缀。\n");
  printf("efilename   输出xml文件的后缀。\n");
  printf("outpath     输出xml文件存放的目录。\n");
  printf("maxcount    输出xml文件的最大记录数，缺省是0，表示无限制，如果本参数取值为0，注意适当加大timeout的取值，防止程序超时。\n");
  printf("starttime   程序运行的时间区间，例如02,13表示：如果程序启动时，踏中02时和13时则运行，其它时间不运行。"
         "如果starttime为空，那么starttime参数将失效，只要本程序启动就会执行数据抽取，为了减少数据源"
         "的压力，从数据库抽取数据的时候，一般在对方数据库最闲的时候时进行。\n");
  printf("incfield    递增字段名，它必须是fieldstr中的字段名，并且只能是整型，一般为自增字段。"
         "如果incfield为空，表示不采用增量抽取方案。");
  printf("incfilename 已抽取数据的递增字段最大值存放的文件，如果该文件丢失，将重新抽取全部的数据。\n");
  printf("connstr1    已抽取数据的递增字段最大值存放的数据库的连接参数。connstr1和incfilename二选一，connstr1优先。");
  printf("timeout     本程序的超时时间，单位：秒。\n");
  printf("pname       进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n");
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

  GetXMLBuffer(strxmlbuffer, "selectsql", starg.selectsql, 1000);
  if (strlen(starg.selectsql) == 0)
  {
    logfile.Write("selectsql is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "fieldstr", starg.fieldstr, 500);
  if (strlen(starg.fieldstr) == 0)
  {
    logfile.Write("fieldstr is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "fieldlen", starg.fieldlen, 500);
  if (strlen(starg.fieldlen) == 0)
  {
    logfile.Write("fieldlen is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "bfilename", starg.bfilename, 30);
  if (strlen(starg.bfilename) == 0)
  {
    logfile.Write("bfilename is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "efilename", starg.efilename, 30);
  if (strlen(starg.efilename) == 0)
  {
    logfile.Write("efilename is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "outpath", starg.outpath, 300);
  if (strlen(starg.outpath) == 0)
  {
    logfile.Write("outpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "starttime", starg.starttime, 50); // 可选参数。

  GetXMLBuffer(strxmlbuffer, "incfield", starg.incfield, 30); // 可选参数。

  GetXMLBuffer(strxmlbuffer, "incfilename", starg.incfilename, 300); // 可选参数。

  GetXMLBuffer(strxmlbuffer, "maxcount", &starg.maxcount); // 可选参数。

  GetXMLBuffer(strxmlbuffer, "connstr1", starg.connstr1, 100); // 可选参数。

  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout); // 进程心跳的超时时间。
  if (starg.timeout == 0)
  {
    logfile.Write("timeout is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50); // 进程名。
  if (strlen(starg.pname) == 0)
  {
    logfile.Write("pname is null.\n");
    return false;
  }
  return true;
}
