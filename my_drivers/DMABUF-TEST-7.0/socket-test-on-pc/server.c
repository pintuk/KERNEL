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
        ret = opensocket(&sockfd, sockname, 1);  //1 --> means server
        if (ret < 0) {
                goto out;
        }
        printf("server: socket opened successfully !\n");
for(i=0; i<10; i++)
{
	exportfd = 4+i; //open("/tmp/DUMMY",O_RDWR);
	printf("server: exportfd: %d\n",exportfd);
        memset(&skdata, 0, sizeof(skdata));
        skdata.data = exportfd;
        skdata.len = sizeof(exportfd);
        ret = sendtosocket(sockfd, &skdata);
        if (ret < 0) {
                goto sockout;
        }
        printf("socketsend - DONE  !\n");
	sleep(1);
}
	sleep(5);

sockout:
	closesocket(sockfd, sockname);
out:
	return 0;

}

