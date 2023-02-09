/*
 * 程序名：selecttable.cpp,演示开发框架操作MySQL数据库（查询）
 * 作者：moonhoro
 */
#include "/root/project/public/db/mysql/_mysql.h"

struct st_girls
{
    long id;
    char name[31];
    double weight;
    char btime[20];
} stgirls;

int main(int argc, char *argv[])
{
    connection conn;

    // 登录数据库

    if (conn.connecttodb("127.0.0.1,root,Ggh.20020902,mysql,3306", "utf8") != 0)
    {
        printf("connect datebase failed.\n%s\n", conn.m_cda.message);
        return -1;
    }
    // 操作SQL语句的对象
    sqlstatement stmt(&conn);

    int iminid, imaxid;
    // 准备查询
    stmt.prepare("\
    select id,name,weight,date_format(btime,'%%Y-%%m-%%d %%H:%%i:%%s') from girls where id>=:1 and id<=:2");
    // 绑定
    stmt.bindin(1, &iminid);
    stmt.bindin(2, &imaxid);
    stmt.bindout(1, &stgirls.id);
    stmt.bindout(2, stgirls.name, 30);
    stmt.bindout(3, &stgirls.weight);
    stmt.bindout(4, stgirls.btime, 19);

    iminid = 1;
    imaxid = 5;
    // 查询数据
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
        return -1;
    }
    while (true)
    {
        memset(&stgirls, 0, sizeof(struct st_girls));
        if (stmt.next() != 0)
            break;
        printf("id=%ld,name=%s,weight=%.02f,btime=%s\n", stgirls.id, stgirls.name, stgirls.weight, stgirls.btime);
    }
    printf("本次查询了girls表%ld条记录\n", stmt.m_cda.rpc);
    return 0;
}