#include "/root/project/public/_ftp.h"

struct st_arg
{
 char host[31];           // 远程服务器的IP和端口。
  int  mode;            // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];       // 远程服务器ftp的用户名。
  char password[31];       // 远程服务器ftp的密码。
  char remotepath[301];    // 远程服务器存放文件的目录。
  char localpath[301];     // 本地文件存放的目录。
  char matchname[101];     // 待上传文件匹配的规则。  
  int ptype;
  char localpathbak[301];
char okfilename[301];
int timeout;
  char pname[51];
}starg;

struct st_fileinfo
{
  char filename[301];
  char mtime[21];
};
vector<struct st_fileinfo> vlistfile1;//已经成功上传文件名的容器
vector<struct st_fileinfo> vlistfile2;//上传前服务器文件名的容器
vector<struct st_fileinfo> vlistfile3;//本次不需要上传的文件容器
vector<struct st_fileinfo> vlistfile4;//本次需要上传的文件容器

bool LoadLocalFile();

CLogFile logfile;

Cftp ftp;

void EXIT(int sig);

void _help();

bool _xmltoarg(char *strxmlbuffer);

bool _ftpputfiles();

bool LoadOKFile();

bool CompVector();

bool WriteToOKFile();

bool AppendToOKFile(struct st_fileinfo *stfileinfo);

CPActive PActive;

int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    _help();
    return -1;
  }

  //关闭信号
//  CloseIOAndSignal();
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);
//打开日志文件
if(logfile.Open(argv[1],"a+")==false)
{
printf("logfile.Open failed.");
return -1;
}
//解析xml
if(_xmltoarg(argv[2])==false)return -1;

//写入心跳信息
PActive.AddPInfo(starg.timeout,starg.pname);
//登录服务器
if(ftp.login(starg.host,starg.username,starg.password,starg.mode)==false)
{
  logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
}

logfile.Write("登录(%s,%s,%s)成功\n",starg.host,starg.username,starg.password);

_ftpputfiles();

ftp.logout();

  return 0;
}

bool _ftpputfiles()
{
//把localpath目录下的文件加载到vlistfiles2容器中
if(LoadLocalFile()==false)
{
  logfile.Write(" LoadLocalFile() failed.\n");
}


PActive.UptATime();

//处理容器
if(starg.ptype==1)
{
//加载okfilename文件中的内容到vlistfile1中
LoadOKFile();
//比较vlistfile2和vlistfile2，得到vlistfile3和vlistfile4
CompVector();
//把容器vlistfile3中的内容写入okfilename文件，覆盖旧okfilename
WriteToOKFile();
//将vlistfile4的内容复制到vlistfile2中
vlistfile2.clear();
vlistfile2.swap(vlistfile4);
}
PActive.UptATime();

//遍历容器vilistfile2
char strremotefilename[301],strlocalfilename[301];
for(int ii=0;ii<vlistfile2.size();ii++)
{
SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vlistfile2[ii].filename);
 SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vlistfile2[ii].filename);   

logfile.Write("put %s ...",strlocalfilename);

if(ftp.put(strlocalfilename,strremotefilename,true)==false)
{
logfile.Write("failed.\n");
return false;
}
logfile.WriteEx("OK.\n");

PActive.UptATime();

if(starg.ptype==1)AppendToOKFile(&vlistfile2[ii]);
//删除
if(starg.ptype==2)
{
  if(REMOVE(strlocalfilename)==false)
  {
    logfile.Write("REMOVE(%s)failed.",strlocalfilename);
    return false;
  }
}
//转存
if(starg.ptype==3)
{
  char strlocalfilenamebak[301];
  SNPRINTF(strlocalfilenamebak,sizeof(strlocalfilenamebak),300,"%s/%s",starg.localpathbak,vlistfile2[ii].filename);
 if(RENAME(strlocalfilename,strlocalfilenamebak)==false) 
{
     logfile.Write("RENAME(%s,%s) failed.\n",strlocalfilename,strlocalfilenamebak);
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
  printf("Using:/root/project/tools1/bin/ftpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/root/project/tools1/bin/procctl 30 /root/project/tools1/bin/ftpputfiles /root/log/idc/ftpputfiles_surfdata.log \"<host>8.130.41.197:21</host><mode>1</mode><username>username</username><password>password</password><localpath>/root/tmp/idc/surfdata</localpath><remotepath>/root/tmp/idc/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>3</ptype><localpathbak>/root/tmp/idc/surfdatabak</localpathbak><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles</pname>\"\n\n\n");
  
  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件上传到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件上传的参数，如下：\n");
  printf("<host>8.130.41.197:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>user</username> 远程服务器ftp的用户名。\n");
  printf("<password>password</password> 远程服务器ftp的密码。\n");
  printf("<remotepath>/root/tmp/idc/ftpputest</remotepath> 远程服务器存放文件的目录。\n");
  printf("<localpath>/root/tmp/idc/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.JSON</matchname> 待上传文件匹配的规则。"\
         "不匹配的文件不会被上传，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
 printf("<ptype>1</ptype> 文件上传成功后，远程服务器文件处理方式：1-什么也不做 2-删除 3-备份 如果为3，还需要指定备份目录\n");
 printf("<localpathbak>/root/tmp/idc/surfdatabak</localpathbak> 文件上传成功后，服务器文件的备份目录，只有当ptype=3时生效\n");
printf("<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename> 上传成功文件清单，当ptype=1时生效\n");
printf("<timeout>80</timeout> 上传文件超时时间 单位：秒\n");
printf("<pname>ftpputfiles</pname> 进程名\n\n\n");
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

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // 待上传文件匹配的规则。
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }
 GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
   if ((starg.ptype!=1)&&(starg.ptype!=2)&&(starg.ptype!=3))
   { logfile.Write("ptype is error.\n");  return false; }
   GetXMLBuffer(strxmlbuffer,"localpathbak",starg.localpathbak,300);
   if ((starg.ptype==3)&&(strlen(starg.localpathbak)==0))
 { logfile.Write("listfilename is null.\n");  return false; }
GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300);
   if ((starg.ptype==1)&&(strlen(starg.okfilename)==0))
   { logfile.Write("okfilename is null.\n");  return false; }
 GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
 if(starg.timeout==0)
 {
   logfile.Write("timeout is null\n");
 return false;
 }
GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);
if(strlen(starg.pname)==0)
{
logfile.Write("pname is null\n");
return false;
}
   return true;
}
bool LoadLocalFile()
{
  vlistfile2.clear();

CDir Dir;

Dir.SetDateFMT("yyyymmddhh24miss");

if(Dir.OpenDir(starg.localpath,starg.matchname)==false)
{
  logfile.Write("Dir.OpenDir(%s) failed\n",starg.localpath);
  return false;
}
  struct st_fileinfo st_fileinfo;

  while(true)
  {
    memset(&st_fileinfo,0,sizeof(struct st_fileinfo));

    if(Dir.ReadDir()==false)break;

    strcpy(st_fileinfo.filename,Dir.m_FileName);
    strcpy(st_fileinfo.mtime,Dir.m_ModifyTime);
   
    vlistfile2.push_back(st_fileinfo);
  }
/*  for(int ii=0;ii<vlistfile2.size();ii++)
    logfile.Write("filename=%s=\n",vlistfile2[ii].filename);
 */
    return true;
}

bool LoadOKFile()
{
  vlistfile1.clear();

  CFile File;

  if((File.Open(starg.okfilename,"r"))==false)return true;

char strbuffer[501];

 struct st_fileinfo st_fileinfo;
 
  while(true)
   {
     memset(&st_fileinfo,0,sizeof(struct st_fileinfo));
                                                                                 
     if(File.Fgets(strbuffer,300,true)==false)break;
  
     GetXMLBuffer(strbuffer,"filename",st_fileinfo.filename);
      GetXMLBuffer(strbuffer,"mtime",st_fileinfo.mtime);
    vlistfile1.push_back(st_fileinfo);
 }

  return true;
}
  
bool CompVector()
{
vlistfile3.clear();vlistfile4.clear();
int ii,jj;
for(ii=0;ii<vlistfile2.size();ii++)
{
  for(jj=0;jj<vlistfile1.size();jj++)
  {
    if(strcmp(vlistfile2[ii].filename,vlistfile1[jj].filename)==0)
      if(strcmp(vlistfile2[ii].mtime,vlistfile1[jj].mtime)==0)
    {
      vlistfile3.push_back(vlistfile2[ii]);
      break;
    }
  }
  if(jj==vlistfile1.size())vlistfile4.push_back(vlistfile2[ii]);
}
}
 
bool WriteToOKFile()
{
  CFile File;

  if(File.Open(starg.okfilename,"w")==false)
  {
logfile.Write("File.Open(%s) failed.\n",starg.okfilename);
return false;
  }

  for(int ii=0;ii<vlistfile3.size();ii++)
  {
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",vlistfile3[ii].filename,vlistfile3[ii].mtime);
  }
  return true;
}
bool AppendToOKFile(struct st_fileinfo *stfileinfo)
{
 CFile File;
 if(File.Open(starg.okfilename,"a")==false)
  {
logfile.Write("File.Open(%s) failed.\n",starg.okfilename);
 return false;
  }
 
File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",stfileinfo->filename,stfileinfo->mtime);
 return true;
 }

