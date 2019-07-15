#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#define TEST_SET_TIME 0x1
int get_rdtsc(){
	asm("rdtsc");
}
unsigned int gethz(){
	unsigned int t1,t2,ret;
	t1=get_rdtsc();
	sleep(5);
	t2=get_rdtsc();
	ret=(abs(t1-t2))/5;
	return ret;
}
int readtime(int fd,unsigned int *a,unsigned int ff)
{
	read(fd,a,2*sizeof(unsigned int));//get basetime
	unsigned int jset1=get_rdtsc();
	unsigned int jpass=abs((jset1-a[0]))/ff;
	a[1]=a[1]+jpass;
	a[0]=jset1;
	ioctl(fd,TEST_SET_TIME,a);
	printf("time now is %ud\n",a[1]);
	return 0;
}


int writetime(int fd,unsigned int *a)
{
	printf("enter time\n");
	scanf("%ud",a+1);
	a[0]=get_rdtsc();
	ioctl(fd,TEST_SET_TIME,a);
	return 0;
}

int main()
{
	unsigned int q=get_kernel_time();
	int fd;
	unsigned int a[2]={0,0};
	unsigned int ff;
	printf("initializing....\n");
	ff=gethz();
	printf("ff=%d\n",ff);
	printf("*************\n");
	fd=open("/dev/globalmem",O_RDWR);
	//write(fd,&a,sizeof(unsigned int));
	readtime(fd,a,ff);
	printf("sleep 5s\n");
	sleep(5);
	readtime(fd,a,ff);
	printf("***********\n");
	writetime(fd,a);
	printf("sleep 10s\n");
	sleep(10);
	readtime(fd,a,ff);
	return 0;
}	
