#include "/root/project/public/_ftp.h"

struct st_arg
{
 char host[31];           // 远程服务器的IP和端口。
  int  mode;            // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];       // 远程服务器ftp的用户名。
  char password[31];       // 远程服务器ftp的密码。
  char remotepath[301];    // 远程服务器存放文件的目录。
  char localpath[301];     // 本地文件存放的目录。
  char matchname[101];     // 待下载文件匹配的规则。  
  char listfilename[301];
  int ptype;
  char remotepathbak[301];

}starg;

struct st_fileinfo
{
  char filename[301];
  char mtime[21];
};

vector<struct st_fileinfo> vfilelist;

bool LoadListFile();

CLogFile logfile;

Cftp ftp;

void EXIT(int sig);

void _help();

bool _xmltoarg(char *strxmlbuffer);

bool _ftpgetfiles();

int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    _help();
  }

  //关闭信号
  //CloseIOAndSignal();
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);
//打开日志文件
if(logfile.Open(argv[1],"a+")==false)
{
printf("logfile.Open failed.");
}

//解析xml
if(_xmltoarg(argv[2])==false)return -1;
//登录服务器
if(ftp.login(starg.host,starg.username,starg.password,starg.mode)==false)
{
  logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
}

logfile.Write("登录(%s,%s,%s)成功\n",starg.host,starg.username,starg.password);

_ftpgetfiles();

ftp.logout();

  return 0;
}

bool _ftpgetfiles()
{
  //进入ftp服务器存放文件的目录
  if(ftp.chdir(starg.remotepath)==false)
  {
    logfile.Write("ftp.chdir(starg.remotepath) false");
    return -1;
  }
//列出服务器目录中的文件，结果放到本地文件
if(ftp.nlist(".",starg.listfilename)==false)
{
  logfile.Write("ftp.nlist(%s) failed.\n",starg.listfilename);
  return false;
}
//把ftp.nlist()获得的目录名加载到容器vfilelist中。
if(LoadListFile()==false)
{
  logfile.Write("LoadListFile failed");
}
//遍历容器vilistfile
char strremotefilename[301],strlocalfilename[301];
  char strremotefilenamebak[301];
for(int ii=0;ii<vfilelist.size();ii++)
{
SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vfilelist[ii].filename);
 SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vfilelist[ii].filename);   

logfile.Write("get %s ...",strremotefilename);

if(ftp.get(strremotefilename,strlocalfilename)==false)
{
logfile.Write("failed.\n");
return false;
}
logfile.WriteEx("OK.\n");
//删除
if(starg.ptype==2)
{
  if(ftp.ftpdelete(strremotefilename)==false)
  {
    logfile.Write("ftp.ftpdelete(%s)failed.",strremotefilename);
    return false;
  }
}
//转存
if(starg.ptype==3)
{
  SNPRINTF(strremotefilenamebak,sizeof(strremotefilenamebak),300,"%s/%s",starg.remotepathbak,vfilelist[ii].filename);
 if(ftp.ftprename(strremotefilename,strremotefilenamebak)==false) 
{
     logfile.Write("ftp.ftprename(%s,%s) failed.\n",strremotefilename,strremotefilenamebak);
      return false;
    }

    }

}
return 0;
}

void EXIT(int sig)
{
  printf("程序退出\n");
  
  exit(0);
}

void _help()
{
   printf("\n");
  printf("Using:/root/project/tools1/bin/ftpgetfiles logfilename xmlbuffer\n\n");

  printf("Sample:/root/project/tools1/bin/procctl 30 /root/project/tools1/bin/ftpgetfiles /root/log/idc/ftpgetfiles_surfdata.log \"<host>8.130.41.197:21</host><mode>1</mode><username>usermoon</username><password>password</password><localpath>/idcdata/surfdata</localpath><remotepath>/root/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>3</ptype><remotepathbak>/root/tmp/idc/surfdatabak</remotepathbak>\"\n\n\n");
  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数，如下：\n");
  printf("<host>8.130.41.197:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>user</username> 远程服务器ftp的用户名。\n");
  printf("<password>password</password> 远程服务器ftp的密码。\n");
  printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务器存放文件的目录。\n");
  printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"\
         "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
  printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> 下载前列出服务器文件名的文件。\n");
 printf("<ptype>1</ptype> 文件下载成功后，远程服务器文件处理方式：1-什么也不做 2-删除 3-备份 如果为3，还需要指定备份目录\n");
 printf("<remotepathbak>/root/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务器文件的备份目录，只有当ptype=3时生效\n\n\n");
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);   // 远程服务器的IP和端口。
  if (strlen(starg.host)==0)
  { logfile.Write("host is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);   // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  if (starg.mode!=2)  starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // 远程服务器ftp的用户名。
  if (strlen(starg.username)==0)
  { logfile.Write("username is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // 远程服务器ftp的密码。
  if (strlen(starg.password)==0)
  { logfile.Write("password is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // 远程服务器存放文件的目录。
  if (strlen(starg.remotepath)==0)
  { logfile.Write("remotepath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // 本地文件存放的目录。
  if (strlen(starg.localpath)==0)
  { logfile.Write("localpath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // 待下载文件匹配的规则。
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }
 GetXMLBuffer(strxmlbuffer,"listfilename",starg.listfilename,300);
  if (strlen(starg.listfilename)==0)
 { logfile.Write("listfilename is null.\n");  return false; }
 GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
   if ((starg.ptype!=1)&&(starg.ptype!=2)&&(starg.ptype!=3))
   { logfile.Write("ptype is error.\n");  return false; }
   GetXMLBuffer(strxmlbuffer,"remotepathbak",starg.remotepathbak,300);
   if ((starg.ptype==3)&&(strlen(starg.remotepathbak)==0))
 { logfile.Write("listfilename is null.\n");  return false; }
  return true;
}
bool LoadListFile()
{
  vfilelist.clear();

  CFile File;

  if(File.Open(starg.listfilename,"r")==false)
  {
    logfile.Write("File.Open(%s) failed",starg.listfilename);
    return false;
  }
  struct st_fileinfo st_fileinfo;

  while(true)
  {
    memset(&st_fileinfo,0,sizeof(struct st_fileinfo));

    if(File.Fgets(st_fileinfo.filename,300,true)==false)break;

    if(MatchStr(st_fileinfo.filename,starg.matchname)==false)continue;
    
    vfilelist.push_back(st_fileinfo);
  }
/*  for(int ii=0;ii<vfilelist.size();ii++)
    logfile.Write("filename=%s=\n",vfilelist[ii].filename);
 */
    return true;
}
