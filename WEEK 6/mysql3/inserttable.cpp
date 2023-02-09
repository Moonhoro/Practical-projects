/*
 * 程序名：inserttable.cpp,演示开发框架操作MySQL数据库（插入五条内容）
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

    stmt.prepare("\
    insert into girls(id,name,weight,btime) values(:1,:2,:3,str_to_date(:4,'%%Y-%%m-%%d %%H:%%i:%%s'))");
    stmt.bindin(1, &stgirls.id);
    stmt.bindin(2, stgirls.name, 30);
    stmt.bindin(3, &stgirls.weight);
    stmt.bindin(4, stgirls.btime, 19);
    for (int ii = 0; ii < 5; ii++)
    {
        memset(&stgirls, 0, sizeof(struct st_girls));

        stgirls.id = ii + 1;
        sprintf(stgirls.name, "%05d貂蝉", ii + 1);
        stgirls.weight = 45.25 + ii;
        sprintf(stgirls.btime, "2021-08-25 10:33:%02d", ii);

        if (stmt.execute() != 0)
        {
            printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
            return -1;
        }
        printf("成功插入%ld条记录\n", stmt.m_cda.rpc);

        printf("insert table girls ok.\n");

        conn.commit();
    }
    return 0;
}