/*
 *  程序名：crtsurfdata1.cpp  本程序用于生成全国气象站点观测的分钟数据。
 *  作者：moonhoro
*/

#include"_public.h"

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

CLogFile logfile(10);

int main(int argc,char *argv[])
{
if(argc!=4)
{
    // 如果参数非法，给出帮助文档。
    printf("Using:./crtsurfdata1 inifile outpath logfile\n");
    printf("Example:/root/project/idc1/bin/crtsurfdata2 /root/project/idc1/ini/stcode.ini /root/tmp/surfdata /root/log/idc/crtsurfdata2.log\n\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名。\n\n");

    return -1;

}

if(logfile.Open(argv[3])==false)
{
printf("logfile.Open(%s) failed.\n",argv[3]);return -1;
}

logfile.Write("crtsurfdata2 开始运行。\n");

//把站点参数文件加载到vstcode容器中。
if(LoadSTCode(argv[1])==false)return -1;


logfile.Write("crtsurfdata2 运行结束。\n"); 
return 0; 
}


//把站点参数文件加载到vstcode容器中。
bool LoadSTCode(const char *inifile)
{
//打开站点文件。
CFile File;
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

logfile.Write("=%s=\n",strBuffer,true);
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
/*
for(int ii=0;ii<vstcode.size();ii++)
logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,vstcode[ii].lon,vstcode[ii].height);
*/

return true;
}
