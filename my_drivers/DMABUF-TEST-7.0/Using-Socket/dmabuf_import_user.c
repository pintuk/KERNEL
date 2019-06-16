#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<errno.h>

#include "ipcsocket.h"

#define IMPORTERB_DEVICE_NAME    "/dev/importerB"
#define SOCKET_NAME             "SOCKET_DMABUF"

#define IMPORTERB_MAGIC         '-'
#define IMPORTERB_BUF_GET       _IOWR(IMPORTERB_MAGIC, 1, int)
#define IMPORTERB_BUF_MAP       _IOWR(IMPORTERB_MAGIC, 2, int)
#define IMPORTERB_BUF_UNMAP     _IOWR(IMPORTERB_MAGIC, 3, int)

#define KB	(1024)
#define MB	(KB*KB)

int main(int argc, char *argv[])
{
	int fd1 = -1;
	int drmfd;
	int exportfd;
	int ret = -1;
	unsigned long len = 0;
	char sockname[20];
	int sockfd;
	struct socketdata skdata;
	int i=0;

	fd1 = open(IMPORTERB_DEVICE_NAME, O_RDWR);
	if (fd1 < 0) {
		perror("::ERROR - opening device : ");
		exit(-1);
	}
//for(i=0; i<10; i++)
//{
	strcpy(sockname, SOCKET_NAME);
	ret = opensocket(&sockfd, sockname, 0);  //0 -> means client
	if (ret < 0) {
		goto out;
	}
	printf("IMPORTER: socket open successfully !\n");
for(i=0; i<10; i++)
{
	memset(&skdata, 0, sizeof(skdata));
	ret = receivefromsocket(sockfd, &skdata);
	if (ret < 0) {
		goto sockout;
	}
	printf("IMPORTER: socket receive - DONE !\n");
	exportfd = (int )skdata.data;
	len = skdata.len;
	printf("IMPORTER: Received fd: %d\n",exportfd);
	ret = ioctl(fd1,IMPORTERB_BUF_GET,&exportfd);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_GET - Failed !\n");
		goto sockout;
	}
	printf("IMPORTERB: BUF_GET - DONE !\n");
	ret = ioctl(fd1,IMPORTERB_BUF_MAP,0);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_MAP - Failed !\n");
		goto sockout;
	}
	printf("IMPORTER: _BUF_MAP - DONE !\n");
	ret = ioctl(fd1,IMPORTERB_BUF_UNMAP,0);
	if (ret < 0) {
		printf("ERROR : ioctl - IMPORTERB_BUF_UNMAP - Failed !\n");
		goto sockout;
	}
	printf("IMPORTER: _BUF_UNMAP - DONE !\n");
	//closesocket(sockfd, sockname);
	//sockfd = -1;
	//sleep(1);
}
	printf("IMPORTER: ------------ WAITING --------------\n");
	sleep(5);

sockout:
	if(sockfd)
	closesocket(sockfd, sockname);

out:
	close(fd1);

	printf("dmabuf: IMPORT - Done !!!\n");

	return 0;
}


