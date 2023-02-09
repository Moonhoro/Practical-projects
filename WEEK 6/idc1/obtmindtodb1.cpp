/*
 *obtmindtodb.cpp,本程序将全国站点每分钟观测数据入库到T_ZHOBMIND表中，支持xml，csv(实现基本框架)
 *作者:moonhoro
 */
#include "/root/project/public/_public.h"
#include "/root/project/public/db/mysql/_mysql.h"
CPActive PActive;

void EXIT(int sig);

CLogFile logfile;

CFile File;

connection conn;

bool _obtmindtodb(char *pathname, char *connstr, char *charset);

int main(int argc, char *argv[])
{
    // 帮助文档。
    if (argc != 5)
    {
        printf("\n");
        printf("Using:./obtmindtodb pathname connstr charset logfile\n");

        printf("Example:/root/project/tools1/bin/procctl 10 /root/project/idc1/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,mysqlpwd,mysql,3306\" utf8 /root/log/idc/obtmindtodb.log\n\n");

        printf("本程序用于把全国站点分钟观测数据保存到数据库的T_ZHOBTMIND表中，数据只插入，不更新。\n");
        printf("pathname 全国站点分钟观测数据文件存放的目录。\n");
        printf("connstr  数据库连接参数：ip,username,password,dbname,port\n");
        printf("charset  数据库的字符集。\n");
        printf("logfile  本程序运行的日志文件名。\n");
        printf("程序每10秒运行一次，由procctl调度。\n\n\n");

        return -1;
    }
    // 处理信号
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    // 打开日志文件
    if (logfile.Open(argv[4], "a+") == false)
    {
        printf("打开日志文件失败(%s)\n", argv[4]);
    }
    PActive.AddPInfo(30, "obtmindtodb");
    // 业务处理主函数
    _obtmindtodb(argv[1], argv[2], argv[3]);
    // 链接数据库
    /*if (conn.connecttodb(argv[2], argv[3]) != 0)
    {
        logfile.Write("connect atabase(%S) failed.\n", argv[2]);
        return -1;
    }
    logfile.Write("connect database(%s) ok.\n", argv[2]);
*/
    // 提交事务
    //  conn.commit();
}
void EXIT(int sig)
{
    logfile.Write("程序退出，sig=%d\n\n", sig);

    conn.disconnect();

    exit(0);
}

bool _obtmindtodb(char *pathname, char *connstr, char *charset)
{
    CDir Dir;
    // 读取目录,取得文件名
    if (Dir.OpenDir(pathname, "*.xml") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed\n", pathname);
        return false;
    }
    while (true)
    {
        if (Dir.ReadDir() == false)
            break;
        logfile.Write("filename=%s\n", Dir.m_FullFileName);
    }
    return true;
}