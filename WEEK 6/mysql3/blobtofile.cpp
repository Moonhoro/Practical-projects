/*
 * 程序名：blobtofile.cpp,演示开发框架操作MySQL数据库（把BLOB字段内容提取到图片文件）
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

    if (conn.connecttodb("127.0.0.1,root,mysqlpassword,mysql,3306", "utf8") != 0)
    {
        printf("connect datebase failed.\n%s\n", conn.m_cda.message);
        return -1;
    }
    // 操作SQL语句的对象
    sqlstatement stmt(&conn);
    // 准备
    stmt.prepare("select id,pic from girls where id in (1,2)");
    // 绑定
    stmt.bindout(1, &stgirls.id);
    stmt.bindoutlob(2, stgirls.pic, 1000000, &stgirls.picsize);
    // 修改数据
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
        return -1;
    }
    while (true)
    {
        memset(&stgirls, 0, sizeof(stgirls));

        if (stmt.next() != 0)
            break;
        char filename[101];
        memset(filename, 0, sizeof(filename));
        sprintf(filename, "%d_out.jpg", stgirls.id);

        buftofile(filename, stgirls.pic, stgirls.picsize);
    }
    printf("本次查询了girls表的%ld条记录\n", stmt.m_cda.rpc);
    return 0;
}
