/*
 * 程序名：filetoblob.cpp,演示开发框架操作MySQL数据库（把图片存入BLOB字段）
 * 作者：moonhoro
 */
#include "/root/project/public/db/mysql/_mysql.h"

struct st_girls
{
    long id;
    char pic[100000];
    unsigned long picsize;
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
    // 准备修改
    stmt.prepare("update girls set pic=:1 where id=:2");
    // 绑定
    stmt.bindinlob(1, stgirls.pic, &stgirls.picsize);
    stmt.bindin(2, &stgirls.id);
    // 修改数据
    for (int ii = 1; ii < 3; ii++)
    {
        memset(&stgirls, 0, sizeof(struct st_girls));

        stgirls.id = ii;
        if (ii == 1)
            stgirls.picsize = filetobuf("1.jpg", stgirls.pic);
        if (ii == 2)
            stgirls.picsize = filetobuf("2.jpg", stgirls.pic);
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