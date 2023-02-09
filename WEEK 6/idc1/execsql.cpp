/*
 *execsql.cpp,运行SQL语句
 *作者:moonhoro
 */
#include "/root/project/public/_public.h"
#include "/root/project/public/db/mysql/_mysql.h"

CPActive PActive;

void EXIT(int sig);

CLogFile logfile;

CFile File;

connection conn;

int main(int argc, char *argv[])
{
    // 帮助文档。
    if (argc != 5)
    {
        printf("\n");
        printf("Using:./execsql sqlfile connstr charset logfile\n");

        printf("Example:/root/project/tools1/bin/procctl 120 /root/project/tools1/bin/execsql /root/project/idc/sql/cleardata.sql \"127.0.0.1,root,Ggh.20020902,mysql,3306\" utf8 /root/log/idc/execsql.log\n\n");

        printf("这是一个工具程序，用于执行一个sql脚本文件。\n");
        printf("sqlfile sql脚本文件名，每条sql语句可以多行书写，分号表示一条sql语句的结束，不支持注释。\n");
        printf("connstr 数据库连接参数：ip,username,password,dbname,port\n");
        printf("charset 数据库的字符集。\n");
        printf("logfile 本程序运行的日志文件名。\n\n");

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
    PActive.AddPInfo(10, "execsql");

    // 链接数据库
    if (conn.connecttodb(argv[2], argv[3], 1) != 0)
    {
        logfile.Write("connect atabase(%S) failed.\n", argv[2]);
        return -1;
    }
    logfile.Write("connect database(%s) ok.\n", argv[2]);

    // 打开SQL文件
    if (File.Open(argv[1], "r") == false)
    {
        logfile.Write("File.Open(%s) failed.\n", argv[1]);
        EXIT(-1);
    }

    char strsql[1001];
    while (true)
    {
        memset(strsql, 0, sizeof(strsql));

        // 读取以;结尾的一行
        if (File.FFGETS(strsql, 1000, ";") == false)
            break;
        // 删掉SQL语句最后的分号
        char *pp = strstr(strsql, ";");
        if (pp == 0)
            continue;
        pp[0] = 0;

        logfile.Write("%s\n", strsql);

        int iret = conn.execute(strsql);

        // 把结果写日志
        if (iret == 0)
            logfile.Write("exec ok(rpc=%d).\n", conn.m_cda.rpc);
        else
            logfile.Write("exec failed(%s).\n", conn.m_cda.message);

        PActive.UptATime();
    }
    logfile.WriteEx("\n");
    return 0;
}
void EXIT(int sig)
{
    logfile.Write("程序退出，sig=%d\n\n", sig);

    conn.disconnect();

    exit(0);
}