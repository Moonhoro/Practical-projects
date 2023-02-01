/*
 * 程序名：tcpputfiles.cpp，此程序用于演示socket通讯的客户端。
 * 作者：moonhoro
*/
#include "../_public.h"

CTcpClient TcpClient;

bool srv000();//心跳
bool srv001();//登录业务

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./tcpputfiles ip port\nExample:./tcpputfiles 8.130.41.197 5005\n\n"); return -1;
  }

if(TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
{
  printf("TcpClient.ConnectToServer(%s,%s) failed\n",argv[1],argv[2]);
}

if(srv001()==false)
{
printf("srv001() failed.\n");
return -1;
}

sleep(3);



for(int ii=3;ii<5;ii++)
{
  if(srv000()==false)break;
  sleep(ii);
}

return 0;
}


bool srv000()
{ 
char buffer[1024];
   
SPRINTF(buffer,sizeof(buffer),"<srvcode>0</srvcode>");
if (TcpClient.Write(buffer)==false)return false;// 向服务端发送请求报文。
 printf("发送：%s\n",buffer);
  
memset(buffer,0,sizeof(buffer));
 if ((TcpClient.Read(buffer))==false)return false; //接收服务端的回应报文。
  printf("接收：%s\n",buffer);
     
 return true;
 }
bool srv001()
{


  char buffer[1024];
 
  // 第3步：与服务端通讯，发送一个报文后等待回复，然后再发下一个报文。
   
    SPRINTF(buffer,sizeof(buffer),"<srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>");
    if (TcpClient.Write(buffer)==false)return false;// 向服务端发送请求报文。
    printf("发送：%s\n",buffer);

    memset(buffer,0,sizeof(buffer));
    if ((TcpClient.Read(buffer))==false)return false; //接收服务端的回应报文。
    printf("接收：%s\n",buffer);
   
    int iretcode=-1;
    GetXMLBuffer(buffer,"retcode",&iretcode);
    if(iretcode!=0)
    {
printf("登录失败。\n");
return false;
    }
printf("登录成功。\n");
return true;
}


