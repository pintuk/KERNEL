#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<errno.h>

#define EXPORTER_DEVICE_NAME	"/dev/exporterA"
#define IMPORTERB_DEVICE_NAME	"/dev/importerB"

#define EXPORTERA_MAGIC		'+'
#define EXPORTERA_ALLOC         _IOWR(EXPORTERA_MAGIC, 1, int)
#define EXPORTERA_BUF_EXPORT    _IOWR(EXPORTERA_MAGIC, 2, int)
#define EXPORTERA_SHARE_FD      _IOWR(EXPORTERA_MAGIC, 3, int)
#define EXPORTERA_FREE          _IOWR(EXPORTERA_MAGIC, 4, int)

#define IMPORTERB_MAGIC   '-'
#define IMPORTERB_BUF_GET       _IOWR(IMPORTERB_MAGIC, 1, int)
#define IMPORTERB_BUF_MAP       _IOWR(IMPORTERB_MAGIC, 2, int)
#define IMPORTERB_BUF_UNMAP     _IOWR(IMPORTERB_MAGIC, 3, int)


#define PAGE_SIZE	(4*1024)
#define KB		(1024)
#define MB		(KB*1024)


int main(int argc, char *argv[])
{
	int fd1 = -1; int fd2 = -1;
	int i = 0;
	int exportfd = -1;
	int ret = -1;
	unsigned long len = 0;

	fd1 = open(EXPORTER_DEVICE_NAME, O_RDWR);
	if (fd1 < 0) {
		perror("::ERROR - opening device : ");
		exit(-1);
	}
	fd2 = open(IMPORTERB_DEVICE_NAME, O_RDWR);
	if (fd2 < 0) {
		perror("::ERROR - opening device : ");
		exit(-1);
	}
	//printf("Enter length of exporter buffer to share(in KB) : ");
	//scanf("%ld",&len);
	//len = (len*KB);
	len = 500*KB;
	
	ret = ioctl(fd1,EXPORTERA_ALLOC,&len);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_ALLOC - Failed, len : %ld !!!\n",len);
		return -1;
	}
	printf("\nEXPORTERA_ALLOC - DONE.....\n");

	ret = ioctl(fd1,EXPORTERA_BUF_EXPORT,0);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_BUF_EXPORT - Failed !\n");
		return -1;
	}
	printf("\nEXPORTERA_BUF_EXPORT - DONE.....\n");

	ret = ioctl(fd1,EXPORTERA_SHARE_FD,&exportfd);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_SHARE_FD - Failed !\n");
		return -1;
	}
	printf("\nEXPORTERA_SHARE_FD - DONE : received fd : %d\n",exportfd);

for(i=0; i<10; i++)
{
#if 1
	ret = ioctl(fd2,IMPORTERB_BUF_GET,&exportfd);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_GET - Failed !\n");
		return -1;
	}
	printf("IMPORTERB_BUF_GET - DONE.....\n");
	
	//printf("Enter length of importer buffer (in KB) : ");
	//scanf("%ld",&len);
	//len = len*KB;

	len = 500*KB;

	ret = ioctl(fd2,IMPORTERB_BUF_MAP,&len);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_MAP - Failed !\n");
		return -1;
	}
	printf("IMPORTERB_BUF_MAP - DONE.....\n");
	ret = ioctl(fd2,IMPORTERB_BUF_UNMAP,0);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_UNMAP - Failed !\n");
		return -1;
	}
	printf("IMPORTERB_BUF_UNMAP - DONE.....\n");
#endif
}


	printf("sleep for 5 seconds....\n");
	sleep(5);

	close(fd1);
	close(fd2);

	return 0;
}

