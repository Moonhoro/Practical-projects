/*
 *obtmindtodb.cpp,本程序将全国站点每分钟观测数据入库到T_ZHOBMIND表中，支持xml，csv(将类独立成为头文件和cpp文件)
 *作者:moonhoro
 */
#include "idcapp.h"

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

        printf("Example:/root/project/tools1/bin/procctl 10 /root/project/idc1/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,Ggh.20020902,mysql,3306\" utf8 /root/log/idc/obtmindtodb.log\n\n");

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
    if (Dir.OpenDir(pathname, "*.xml,*.csv") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed\n", pathname);
        return false;
    }
    CZHOBTMIND ZHOBTMIND(&conn, &logfile);
    int totalcount = 0;  // 文件记录总数
    int insertcount = 0; // 成功插入数
    CTimer Timer;
    bool isxml = false;
    while (true)
    {
        if (Dir.ReadDir() == false)
            break;
        if (MatchStr(Dir.m_FullFileName, "*.xml") == true)
            isxml = true;
        else
            isxml = false;
        // 链接数据库
        if (conn.m_state == 0)
        {
            if (conn.connecttodb(connstr, charset) != 0)
            {
                logfile.Write("connect database(%s)failed.\n", connstr);
                return -1;
            }
            logfile.Write("connect database(%s) ok.\n", connstr);
        }

        totalcount = 0;
        insertcount = 0;
        // 打开文件
        if (File.Open(Dir.m_FullFileName, "r") == false)
        {
            logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName);
            return false;
        }
        char strBuffer[1001]; // 存放文件中读取的一行
        while (true)
        {
            if (isxml == true)
            {
                if (File.FFGETS(strBuffer, 1000, "<endl/>") == false)
                    break;
            }
            else
            {
                if (File.Fgets(strBuffer, 1000, true) == false)
                    break;
                if (strstr(strBuffer, "站点") != 0)
                    continue;
            }
            // logfile.Write("strBuffer=%s", strBuffer);
            totalcount++;
            ZHOBTMIND.SplitBuffer(strBuffer, isxml);
            if (ZHOBTMIND.InsertTable() == true)
            {
                insertcount++;
            }
        }
        // 删除文件提交事务
        File.CloseAndRemove();
        conn.commit();
        logfile.Write("已处理文件%s(totalcount=%d,insertcount=%d,耗时%.2f)\n", Dir.m_FullFileName, totalcount, insertcount, Timer.Elapsed());
        return true;
    }
}