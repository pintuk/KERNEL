#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<errno.h>

#include "ipcsocket.h"

#define SOCKET_NAME             "SOCKET_DUMY"


int main(int argc, char *argv[])
{
	int fd1 = -1;
	char sockname[20];
	int sockfd;
	int exportfd;
	struct socketdata skdata;
	int ret;
	int i=0;

	strcpy(sockname, SOCKET_NAME);
        ret = opensocket(&sockfd, sockname, 0);  //1 --> means server
        if (ret < 0) {
                goto out;
        }
        printf("client: socket opened successfully !\n");
for(i=0; i<10; i++)
{
	//exportfd = 4;
        memset(&skdata, 0, sizeof(skdata));
        //skdata.data = exportfd;
        //skdata.len = sizeof(exportfd);
        ret = receivefromsocket(sockfd, &skdata);
        if (ret < 0) {
                goto sockout;
        }
        printf("socketreceive - DONE  !\n");
	exportfd = skdata.data;
	printf("fd received: %d\n",exportfd);
	sleep(1);
}

sockout:
	closesocket(sockfd, sockname);
out:
	return 0;

}

