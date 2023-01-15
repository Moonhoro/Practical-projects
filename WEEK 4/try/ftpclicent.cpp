#include "/root/project/public/_ftp.h"
Cftp ftp;
int main()
{
  if(ftp.login("8.130.41.197:21","用户名","密码")==false)
  {
    printf("ftp.login(8.130.41.197:21) failed.\n");
    return -1;
  }
  printf("ftp.login(8.130.41.197:21) ok.\n");


  if(ftp.mtime("/home/moon/test/_public.h")==false)
  {
    printf("ftp.mtime(/home/moon/test/_public.h failed.\n");
    return -1;
  }
   printf("ftp.mtime(/home/moon/test/_public.h ok.,mtime=%s.\n",ftp.m_mtime);

  if(ftp.size("/home/moon/test/_public.h")==false)
   {
   printf("ftp.size(/home/moon/test/_public.h) failed.\n");
   return -1;
   }
  printf("ftp.size(/home/moon/test/_public.h) ok,size=%d.\n",ftp.m_size);
  
  if(ftp.nlist("/home/moon/test","/root/tmp/aaa/bbb.lst")==false)
  {
    printf("ftp.nlist(/home/moon/test,/root/tmp/aaa/bbb.lst) failed\n");
  }
   printf("ftp.nlist(/home/moon/test,/root/tmp/aaa/bbb.lst) ok\n");
 
if(ftp.get("/home/moon/test/_public.cpp","/root/tmp/ftptesr/_public.cpp.bak",true)==false)
{
  printf("ftp.get() failed.\n");
  return -1;
}
printf("ftp.get() ok.\n");

if(ftp.put("/home/moon/test/_public.h","/home/moon/test/_public.h.bak",true)==false)
{
 printf("ftp.put() failed.\n");
   return -1;
 }
 printf("ftp.put() ok.\n"); 


  ftp.logout();
  return 0;
}
