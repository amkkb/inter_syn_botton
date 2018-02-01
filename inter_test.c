#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

int fd;		//定义文件描述符
void inter_func(int sig)
{
	int ret;
	unsigned char key_val = 0;
	read(fd,&key_val,1);
	printf("key_val = 0x%x\n",key_val);
}

int main(int argc, char **argv)
{
    int flag = 0;
    fd = open("/dev/key_driver",O_RDWR);
    if(fd < 0)
    {   
         printf("open device failed\n");
         return -1;    
    }
    else
    {
        printf("open success\n");
    }
		signal(SIGIO,inter_func);//通过signal函数设置信号处理函数inter_func

		fcntl(fd,F_SETOWN,getpid());//通过F_SETOWN命令设置信号接收处理进程
		flag = fcntl(fd,F_GETFL);
		fcntl(fd,F_SETFL,flag|FASYNC);//进行异步模式的设置，这里会导致驱动函数syn的运行	
    while(1)
    {
			 sleep(100);
    }

    return 0;

}
