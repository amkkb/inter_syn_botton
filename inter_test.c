#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

int fd;		//�����ļ�������
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
		signal(SIGIO,inter_func);//ͨ��signal���������źŴ�����inter_func

		fcntl(fd,F_SETOWN,getpid());//ͨ��F_SETOWN���������źŽ��մ������
		flag = fcntl(fd,F_GETFL);
		fcntl(fd,F_SETFL,flag|FASYNC);//�����첽ģʽ�����ã�����ᵼ����������syn������	
    while(1)
    {
			 sleep(100);
    }

    return 0;

}
