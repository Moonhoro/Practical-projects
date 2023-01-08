#include</root/project/public/_public.h>

void EXIT(int sig)
{
  printf("sig=%d\n",sig);
// if(sig==2)exit(0);
exit(0);
}

  int main( int argc,char*argv[] )
{
  CPActive Active;
  if(argc!=3)
  {
    printf("Using:./try4 procname\n");
    return 0;
  }
signal(2,EXIT);signal(15,EXIT);

  Active.AddPInfo(atoi(argv[2]),argv[1]);
  while(true)
  {
   // Active.UptATime();
    sleep(10);
  }
  return 0;
}
