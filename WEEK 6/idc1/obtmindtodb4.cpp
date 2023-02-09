/*
 *obtmindtodb.cpp,本程序将全国站点每分钟观测数据入库到T_ZHOBMIND表中，支持xml，csv(增加类的编写)
 *作者:moonhoro
 */
#include "/root/project/public/_public.h"
#include "/root/project/public/db/mysql/_mysql.h"

struct st_zhobtmind
{
    char obtid[11];     // 站点代码。
    char ddatetime[21]; // 数据时间，精确到分钟。
    char t[11];         // 温度，单位：0.1摄氏度。
    char p[11];         // 气压，单位：0.1百帕。
    char u[11];         // 相对湿度，0-100之间的值。
    char wd[11];        // 风向，0-360之间的值。
    char wf[11];        // 风速：单位0.1m/s。
    char r[11];         // 降雨量：0.1mm。
    char vis[11];       // 能见度：0.1米。
};

// 全国站点分钟观测数据操作类。
class CZHOBTMIND
{
public:
    connection *m_conn;  // 数据库连接。
    CLogFile *m_logfile; // 日志。

    sqlstatement m_stmt; // 插入表操作的sql。

    char m_buffer[1024];             // 从文件中读到的一行。
    struct st_zhobtmind m_zhobtmind; // 全国站点分钟观测数据结构。

    CZHOBTMIND();
    CZHOBTMIND(connection *conn, CLogFile *logfile);

    ~CZHOBTMIND();

    void BindConnLog(connection *conn, CLogFile *logfile); // 把connection和CLogFile的传进去。
    bool SplitBuffer(char *strBuffer);                     // 把从文件读到的一行数据拆分到m_zhobtmind结构体中。
    bool InsertTable();                                    // 把m_zhobtmind结构体中的数据插入到T_ZHOBTMIND表中。
};

CZHOBTMIND::CZHOBTMIND()
{
    m_conn = 0;
    m_logfile = 0;
}
CZHOBTMIND::CZHOBTMIND(connection *conn, CLogFile *logfile)
{
    m_conn = conn;
    m_logfile = logfile;
}
CZHOBTMIND::~CZHOBTMIND()
{
}

bool CZHOBTMIND::SplitBuffer(char *strBuffer)
{
    memset(&m_zhobtmind, 0, sizeof(struct st_zhobtmind));
    GetXMLBuffer(strBuffer, "obtid", m_zhobtmind.obtid, 10);
    GetXMLBuffer(strBuffer, "ddatetime", m_zhobtmind.ddatetime, 14);
    char tmp[11];
    GetXMLBuffer(strBuffer, "t", tmp, 10);
    if (strlen(tmp) > 0)
        snprintf(m_zhobtmind.t, 10, "%d", (int)(atof(tmp) * 10));
    GetXMLBuffer(strBuffer, "p", tmp, 10);
    if (strlen(tmp) > 0)
        snprintf(m_zhobtmind.p, 10, "%d", (int)(atof(tmp) * 10));
    GetXMLBuffer(strBuffer, "u", m_zhobtmind.u, 10);
    GetXMLBuffer(strBuffer, "wd", m_zhobtmind.wd, 10);
    GetXMLBuffer(strBuffer, "wf", tmp, 10);
    if (strlen(tmp) > 0)
        snprintf(m_zhobtmind.wf, 10, "%d", (int)(atof(tmp) * 10));
    GetXMLBuffer(strBuffer, "r", tmp, 10);
    if (strlen(tmp) > 0)
        snprintf(m_zhobtmind.r, 10, "%d", (int)(atof(tmp) * 10));
    GetXMLBuffer(strBuffer, "vis", tmp, 10);
    if (strlen(tmp) > 0)
        snprintf(m_zhobtmind.vis, 10, "%d", (int)(atof(tmp) * 10));
    STRCPY(m_buffer, sizeof(m_buffer), strBuffer);
    return true;
}
bool CZHOBTMIND::InsertTable()
{
    if (m_stmt.m_state == 0)
    {
        m_stmt.connect(m_conn);
        m_stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9,now())");
        m_stmt.bindin(1, m_zhobtmind.obtid, 10);
        m_stmt.bindin(2, m_zhobtmind.ddatetime, 14);
        m_stmt.bindin(3, m_zhobtmind.t, 10);
        m_stmt.bindin(4, m_zhobtmind.p, 10);
        m_stmt.bindin(5, m_zhobtmind.u, 10);
        m_stmt.bindin(6, m_zhobtmind.wd, 10);
        m_stmt.bindin(7, m_zhobtmind.wf, 10);
        m_stmt.bindin(8, m_zhobtmind.r, 10);
        m_stmt.bindin(9, m_zhobtmind.vis, 10);
    }
    if (m_stmt.execute() != 0)
    {
        if (m_stmt.m_cda.rc != 1062)
        {
            m_logfile->Write("Buffer=%s\n", m_buffer);
            m_logfile->Write("stmt.execute() failed.\n%s\n%s\n", m_stmt.m_sql, m_stmt.m_cda.message);
        }
        return false;
    }
    return true;
}

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
    if (Dir.OpenDir(pathname, "*.xml") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed\n", pathname);
        return false;
    }
    CZHOBTMIND ZHOBTMIND(&conn, &logfile);
    int totalcount = 0;  // 文件记录总数
    int insertcount = 0; // 成功插入数
    CTimer Timer;
    while (true)
    {
        if (Dir.ReadDir() == false)
            break;
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
            if (File.FFGETS(strBuffer, 1000, "<endl/>") == false)
                break;
            // logfile.Write("strBuffer=%s", strBuffer);
            totalcount++;
            ZHOBTMIND.SplitBuffer(strBuffer);
            if (ZHOBTMIND.InsertTable() == true)
            {
                insertcount++;
            }
            // 删除文件提交事务
            // File.CloseAndRemove();
            conn.commit();
            logfile.Write("已处理文件%s(totalcount=%d,insertcount=%d,耗时%.2f)\n", Dir.m_FullFileName, totalcount, insertcount, Timer.Elapsed());
        }

        return true;
    }
}