/*
 * 程序名：createtable.cpp,演示开发框架操作MySQL数据库（创建表）
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

  // sqlstatement stmt;
  // stmt.connect(&conn);
  // 超女表girls， 超女编号id 超女姓名name 体重weight 报名时间btime 超女说明memo 超女图片pic
  stmt.prepare("\
    create table girls(id    bigint(10),\
                       name  varchar(30),\
                       weight   decimal(8,2),\
                       btime datetime,\
                       memo  longtext,\
                       pic   longblob,\
                       primary key (id))");

  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%d\n%s\n", stmt.m_sql, stmt.m_cda.rc, stmt.m_cda.message);
    return -1;
  }

  printf("create table girls ok.\n");

  return 0;
}
