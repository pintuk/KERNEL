#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<errno.h>

#include "ipcsocket.h"

#define EXPORTER_DEVICE_NAME    "/dev/exporterA"
#define SOCKET_NAME		"SOCKET_DMABUF"

#define EXPORTERA_MAGIC         '+'
#define EXPORTERA_ALLOC         _IOWR(EXPORTERA_MAGIC, 1, int)
#define EXPORTERA_BUF_EXPORT    _IOWR(EXPORTERA_MAGIC, 2, int)
#define EXPORTERA_SHARE_FD      _IOWR(EXPORTERA_MAGIC, 3, int)
#define EXPORTERA_FREE          _IOWR(EXPORTERA_MAGIC, 4, int)


#define KB	(1024)
#define MB	(KB*KB)

int main(int argc, char *argv[])
{
	int fd1 = -1;
	int exportfd = -1;
	int ret = -1;
	unsigned long len = 0;
	char sockname[20];
	int pair[2];
	int sockfd;
	struct socketdata skdata;
	int i=0;

	fd1 = open(EXPORTER_DEVICE_NAME, O_RDWR);
	if (fd1 < 0) {
		perror("::ERROR - opening device : ");
		exit(-1);
	}
#if 1
	strcpy(sockname, SOCKET_NAME);
	ret = opensocket(&sockfd, sockname, 1);  //1 --> means server
	if (ret < 0) {
		goto out;
	}
	printf("EXPORTERA: server: socket opened successfully !\n");
#endif

	len = 10*KB;
	ret = ioctl(fd1,EXPORTERA_ALLOC,&len);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_ALLOC - Failed, len : %ld !!!\n",len);
		goto out;
	}
	printf("EXPORTERA: ALLOC - DONE.....\n");

for(i=0; i<10; i++)
{
	exportfd = 0;
	ret = ioctl(fd1,EXPORTERA_BUF_EXPORT,0);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_BUF_EXPORT - Failed !\n");
		goto out;
	}
	printf("EXPORTERA: EXPORT - DONE.....\n");

	ret = ioctl(fd1,EXPORTERA_SHARE_FD,&exportfd);
	if (ret < 0) {
		printf("ERROR : ioctl - EXPORTERA_SHARE_FD - Failed !\n");
		goto out;
	}
	printf("EXPORTERA: SHARE_FD: exported fd : %d\n",exportfd);

#if 0
	strcpy(sockname, SOCKET_NAME);
	ret = opensocket(&sockfd, sockname, 1);  //1 --> means server
	if (ret < 0) {
		goto out;
	}
	printf("EXPORTERA: server: socket opened successfully !\n");
#endif
//for(i=0; i<10; i++)
//{
	memset(&skdata, 0, sizeof(skdata));
	skdata.data = exportfd;
	skdata.len = sizeof(exportfd);
	ret = sendtosocket(sockfd, &skdata);
	if (ret < 0) {
		goto sockout;
	}
	printf("EXPORTERA: socketsend - DONE  !\n");
	//closesocket(sockfd, sockname);
	//sockfd = -1;
	//sleep(1);
}
	printf("---------------- EXPORTER WAITING -------------------\n");
	sleep(10);

sockout:
	if(sockfd)
	closesocket(sockfd, sockname);

out:
	close(fd1);

	printf("dmabuf: EXPORTER - TERMINATED!!!\n");

	return 0;
}


