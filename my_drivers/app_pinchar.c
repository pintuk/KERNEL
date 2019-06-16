#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>


#define DEVICE_NAME	"/dev/pinchar"
//#define PINCHAR_ALLOC	1
//#define PINCHAR_FREE	2

#define PINCHAR_MAGIC   'k'
#define PINCHAR_ALLOC   _IOWR(PINCHAR_MAGIC, 1, int)
#define PINCHAR_FREE    _IOWR(PINCHAR_MAGIC, 2, int)


#define PAGE_SIZE	(4*1024)
//#define BLOCK_SIZE	(64)
//#define MAX_BLOCK	(10)	

int main()
{
	int fd = -1; int i = 0;
	char msg[] = "Hello pinchar...\0";
	//char *buffer=NULL; char ch;
	int ret=-1;
	int blksize = 0;
	int maxblock = 0;
	unsigned long len = 0;

	fd = open(DEVICE_NAME, O_RDWR);
	if(fd < 0)
	{
		perror("::ERROR - opening device");
		exit(-1);
	}
	//system("cat /proc/buddyinfo ; cat /proc/zoneinfo | grep free_pages ; cat /proc/pagetypeinfo");
	system("cat /proc/buddyinfo");
	printf("\n");
	system("free -tm");
	//system("cat /proc/pagetypeinfo | grep -m 1 'Free pages count' ; cat /proc/pagetypeinfo | grep 'Node    2' ; cat /proc/pagetypeinfo | grep -m 1 'Number of blocks type' ; cat /proc/pagetypeinfo | grep 'Node 2'");
	printf("-------------------------------------------------------------------------------------------\n");
	printf("\n\n\nEnter the page order(in power of 2) : ");
	scanf("%d",&blksize);
	printf("Enter the number of such block : ");
	scanf("%d",&maxblock);
	for(i=0; i<maxblock; i++)
	{
		system("cat /proc/buddyinfo");
		printf("\n");
		system("free -tm");
		printf("===============================\n");
		len = blksize*PAGE_SIZE;
		ret = ioctl(fd,PINCHAR_ALLOC,len);
		if(ret < 0)
		{
			printf("ERROR : ioctl - PINCHAR_ALLOC - Failed, after block num = %d !!!\n",i);
			break;
		}
		sleep(2);  //This sleep is required to allocate block from desired order after each itteration
		system("cat /proc/buddyinfo");
		printf("\n");
		system("free -tm");
	}
	printf("\nDONE.....\n\n\n");
	/* 
	buddyinfo needs sometime to update itself as soon as the blocks are allocated. T
	hus this sleep is required 
	*/
	sleep(1);
	printf("==========================================================================================\n");
	//system("cat /proc/buddyinfo ; cat /proc/zoneinfo | grep free_pages ; cat /proc/pagetypeinfo");
	system("cat /proc/buddyinfo");
	printf("\n");
	system("free -tm");
	//system("cat /proc/pagetypeinfo | grep -m 1 'Free pages count' ; cat /proc/pagetypeinfo | grep 'Node    2' ; cat /proc/pagetypeinfo | grep -m 1 'Number of blocks type' ; cat /proc/pagetypeinfo | grep 'Node 2'");
	printf("==========================================================================================\n");
	sleep(1);

	for(i=0; i<maxblock; i++)
	{
		ret = ioctl(fd,PINCHAR_FREE,len);
	}
	printf("\n\nFree....Done !!!\n");
	printf("After Free.......buddyinfo is :-\n");
	//system("cat /proc/buddyinfo ; cat /proc/zoneinfo | grep free_pages ; cat /proc/pagetypeinfo");
	system("cat /proc/buddyinfo");
	printf("\n");
	system("free -tm");

	close(fd);

	return(0);
}


