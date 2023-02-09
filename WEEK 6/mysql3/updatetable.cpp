/*
 * 程序名：wupdatetable.cpp,演示开发框架操作MySQL数据库（修改）
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

    if (conn.connecttodb("127.0.0.1,root,mysqlpassword,mysql,3306", "utf8") != 0)
    {
        printf("connect datebase failed.\n%s\n", conn.m_cda.message);
        return -1;
    }
    // 操作SQL语句的对象
    sqlstatement stmt(&conn);
    // 准备修改
    stmt.prepare("\
    update girls set name=:1,weight=:2,btime=str_to_date(:3,'%%Y-%%m-%%d %%H:%%i:%%s') where id=:4");
    // 绑定
    stmt.bindin(1, stgirls.name, 30);
    stmt.bindin(2, &stgirls.weight);
    stmt.bindin(3, stgirls.btime, 19);
    stmt.bindin(4, &stgirls.id);
    // 修改数据
    for (int ii = 0; ii < 5; ii++)
    {
        memset(&stgirls, 0, sizeof(struct st_girls));

        stgirls.id = ii + 1;
        sprintf(stgirls.name, "%05d纳西妲", ii + 1);
        stgirls.weight = 45.28 + ii;
        sprintf(stgirls.btime, "2022-08-25 10:33:%02d", ii);

        if (stmt.execute() != 0)
        {
            printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
            return -1;
        }
        printf("成功修改%ld条记录\n", stmt.m_cda.rpc);

                conn.commit();
    }
    printf("update table girls ok.\n");
    return 0;
}
