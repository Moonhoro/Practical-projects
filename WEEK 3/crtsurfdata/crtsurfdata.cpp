
/*
 *  程序名：crtsurfdata.cpp  本程序用于生成全国气象站点观测的分钟数据。
 *  作者：moonhoro
*/

#include"/root/project/public/_public.h"

CPActive PActive;//进程心跳

struct st_stcode
{
char provname[31];  //省份
char obtid[11]; //站号
char obtname[31]; //站名
double lat; //纬度
double lon; //经度
double height; //海拔高度
};

//存放全国站点参数容器
vector<struct st_stcode> vstcode;

//把站点参数加载到vtcode容器中。
bool LoadSTCode(const char *inifile);

struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
};

vector<struct st_surfdata> vsurfdata;//存放全国气象站点分钟观察数据的容器。

char strddatetime[21];

CFile File;

void CrtSurfData();

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt);

CLogFile logfile(10);

void EXIT(int sig);//信号处理函数

int main(int argc,char *argv[])
{
if((argc!=5)&&(argc!=6))
{
    // 如果参数非法，给出帮助文档。
    printf("Using:./crtsurfdata1 inifile outpath logfile datafmt [datetime]\n");
    printf("Example:/root/project/idc1/bin/crtsurfdata /root/project/idc1/ini/stcode.ini /root/tmp/surfdata /root/log/idc/crtsurfdata.log xml,json,csv 20220101123000\n\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名。\n\n");
     printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。\n\n");
 printf("ddatetime 这是一个可选参数，表示生成指定的时间和数据文件。\n\n");
    return -1;

}
//信号处
CloseIOAndSignal(true);
signal(SIGINT,EXIT);
signal(SIGTERM,EXIT);

if(logfile.Open(argv[3])==false)
{
printf("logfile.Open(%s) failed.\n",argv[3]);return -1;
}

logfile.Write("crtsurfdata 开始运行。\n");

PActive.AddPInfo(20,"crtsurfdata");
//把站点参数文件加载到vstcode容器中。
if(LoadSTCode(argv[1])==false)return -1;

//获取当前时间

memset(strddatetime,0,sizeof(strddatetime));
if(argc==5)
LocalTime(strddatetime,"yyyymmddhh24miss");
else 
STRCPY(strddatetime,sizeof(strddatetime),argv[5]);
//模拟生成全国气象站点每分钟观测数据，放在vstcode容器中。
CrtSurfData();

//把容器vsurfdata中的气象站点分钟观测数据写入文件。
if(strstr(argv[4],"xml")!=0)CrtSurfFile(argv[2],"xml");
if(strstr(argv[4],"json")!=0)CrtSurfFile(argv[2],"json");
if(strstr(argv[4],"csv")!=0)CrtSurfFile(argv[2],"csv");



logfile.Write("crtsurfdata 运行结束。\n");
return 0; 
}


//把站点参数文件加载到vstcode容器中。
bool LoadSTCode(const char *inifile)
{
//打开站点文件。
if(File.Open(inifile,"r")==false)
{
logfile.Write("File.Open(%s) failed.\n",inifile);return false;
}


char strBuffer[301];

CCmdStr CmdStr;

struct st_stcode stcode;

while(true)
{
//从站点参数文件中读取一行，如果已经读完，跳出循环。
if(File.Fgets(strBuffer,300,true)==false)break;

//logfile.Write("=%s=\n",strBuffer,true);
//把读取到的一行进行拆分。
CmdStr.SplitToCmd(strBuffer,",");
if(CmdStr.CmdCount()!=6)continue;//扔掉无效行
//放入结构体
CmdStr.GetValue(0,stcode.provname,30);
CmdStr.GetValue(1,stcode.obtid,10);
CmdStr.GetValue(2,stcode.obtname,30);
CmdStr.GetValue(3,&stcode.lat);
CmdStr.GetValue(4,&stcode.lon);
CmdStr.GetValue(5,&stcode.height);
//放入容器
vstcode.push_back(stcode);
}
return true;
}

void CrtSurfData()
{
//播下随机数
srand(time(0));



//遍历容器
struct st_surfdata stsurfdata;
for(int ii=0;ii<vstcode.size();ii++)
{
memset(&stsurfdata,0,sizeof(struct st_surfdata));
strncpy(stsurfdata.obtid,vstcode[ii].obtid,10);
strncpy(stsurfdata.ddatetime,strddatetime,14);
stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
    stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
    stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
    stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
    stsurfdata.r=rand()%16;        // 降雨量：0.1mm
    stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米
//把结构体放进容器
vsurfdata.push_back(stsurfdata);
 }
}
//把容器vsurfdata中的全球气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt)
{
CFile File;
char strFileName[301];
sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);
//打开文件
if(File.OpenForRename(strFileName,"w")==false)
{
logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
}
//写入第一行标题
if (strcmp(datafmt,"csv")==0) File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
if(strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");
if(strcmp(datafmt,"json")==0) File.Fprintf("{\"data\":[\n");
//写入一条记录
for(int ii=0;ii<vsurfdata.size();ii++)
{
if(strcmp(datafmt,"csv")==0)
{
File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
}
if(strcmp(datafmt,"xml")==0)
{
File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t>,<p>%.1f</p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
}
if(strcmp(datafmt,"json")==0)
{
File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\",\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
if(ii<vsurfdata.size()-1)File.Fprintf(",\n");
else File.Fprintf("\n");
}
}
if(strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
if(strcmp(datafmt,"json")==0) File.Fprintf("]}\n");
File.CloseAndRename();

UTime(strFileName,strddatetime);//修改文件的时间属性。

logfile.Write("生产数据文件%s成功，数据时间%s,记录数%d。\n",strFileName,strddatetime,vsurfdata.size());
return true;
}
void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);
  exit(0);
}
