#include"logmem.h"  
void* logmem(void *p)
{
    char name[64]={'\0'};
    int maxuse=0;
    int memuse=0;
    int linenum=11;
    char file[100]={'\0'};
    FILE * fd;
    sprintf(file,"/proc/%d/status",*((pid_t *)p));
    fd=fopen(file,"r");
    char linebuf[256]={'\0'};
    while(1)
    {
		fd=fopen(file,"r");
        if(fd==NULL)
             break;
        for(int i=0;i<linenum;i++)
           if(!fgets(linebuf,sizeof(linebuf),fd))
                break;
        sscanf(linebuf,"%s %d",name,&memuse);
        if(memuse>maxuse)
            maxuse=memuse;
 	    FILE * re=fopen("mem_use_result.txt","w");
        fprintf(re,"%dMB",maxuse/1024);
        fflush(re);
        sleep(1);
        if(fclose(fd)!=0)
            break;
     }
 return 0;

}
