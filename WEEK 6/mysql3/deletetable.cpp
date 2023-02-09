/*
 * 程序名：deletetable.cpp,演示开发框架操作MySQL数据库（删除）
 * 作者：moonhoro
 */
#include "/root/project/public/db/mysql/_mysql.h"

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
    // 准备删除
    stmt.prepare("\
    delete from girls where id>=:1 and id<=:2");
    // 绑定
    stmt.bindin(1, &iminid);
    stmt.bindin(2, &imaxid);

    iminid = 1;
    imaxid = 5;
    // 查询数据
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
        return -1;
    }
    printf("本次删除了girls表%ld条记录\n", stmt.m_cda.rpc);
    conn.commit();
    return 0;
}