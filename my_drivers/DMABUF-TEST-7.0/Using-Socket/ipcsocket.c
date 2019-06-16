#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#include "ipcsocket.h"
char sock_name[50];

int opensocket(int *sockfd, const char *name, int connecttype)
{
	int ret = 0;
    	int temp = 1;

	ret = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (ret < 0) {
		fprintf(stderr,"ERROR: opensocket: socket - Failed <%s>\n",strerror(errno));
		return -1;
	}
	*sockfd = ret;
	#if 1
    	if(setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&temp, sizeof(int)) < 0) {
		fprintf(stderr,"ERROR: opensocket: setsockopt - Failed <%s>\n",strerror(errno));
		return -1;
    	}
	#endif
	if(strlen(name) > 50) return -1;
	sprintf(sock_name, "/tmp/%s", name);
	if(connecttype == 1) {
		struct sockaddr_un skaddr;
		int clientfd;
		socklen_t sklen;

		printf("---------- SERVER Open --------------\n");
		#if 0
    		if(setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&temp, sizeof(int)) < 0) {
			fprintf(stderr,"ERROR: opensocket: setsockopt - Failed <%s>\n",strerror(errno));
			close(*sockfd);
			return -1;
    		}
		#endif
		unlink(sock_name);
		memset(&skaddr, 0, sizeof(skaddr));
		skaddr.sun_family = AF_LOCAL;
		strcpy(skaddr.sun_path, sock_name);
		printf("SERVER: socket name: %s\n",sock_name);
		ret = bind(*sockfd, (struct sockaddr *)&skaddr, SUN_LEN(&skaddr));
		if (ret < 0) {
			fprintf(stderr,"ERROR: opensocket: bind - Failed <%s>\n",strerror(errno));
			close(*sockfd);
			return -1;
		}
		#if 1
		ret = listen(*sockfd, 5);
		if (ret < 0) {
			fprintf(stderr,"ERROR: opensocket: listen - Failed <%s>\n",strerror(errno));
			return -1;
		}
		memset(&skaddr, 0, sizeof(skaddr));
		sklen = sizeof(skaddr);
		clientfd = accept(*sockfd,(struct sockaddr *)&skaddr,(socklen_t *)&sklen);
		if (ret < 0) {
			fprintf(stderr,"ERROR: sendtosocket: accept - Failed <%s>\n",strerror(errno));
			return -1;
		}
		*sockfd = clientfd;
		#endif
	}
	else {
		struct sockaddr_un skaddr;
		printf("---------- CLIENT open -------------\n");
		memset(&skaddr, 0, sizeof(skaddr));
		skaddr.sun_family = AF_LOCAL;
		strcpy(skaddr.sun_path, sock_name);
		printf("CLIENT: Socket Name: %s\n",(char *)skaddr.sun_path);
		#if 1
		ret = connect(*sockfd, (struct sockaddr *)&skaddr, SUN_LEN(&skaddr));
		if (ret < 0) {
			fprintf(stderr,"ERROR: opensocket: connect - Failed, errno: %d <%s>\n",errno,strerror(errno));
			close(*sockfd);
			return -1;
		}
		#endif
	}
	return 0;
}

int sendtosocket(int sockfd, struct socketdata *skdata)
{
	int ret = 0;
	char *buf;
	unsigned len = 0;
	struct sockaddr_un skaddr;
	socklen_t sklen;
	int clientfd;
	int buffd;
	char cmsg_b[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct msghdr msgh;
	struct iovec iov;
	int temp = 1;
	fd_set selFDs;

	if (!skdata) {
		fprintf(stderr,"ERROR: sendtosocket: socketdata is NULL\n");
		return -1;
	}
	//printf("Entry -> sendtosocket\n");
	FD_ZERO(&selFDs);
	FD_SET(0,&selFDs);
	FD_SET(sockfd,&selFDs);
	ret = select(sockfd+1,NULL,&selFDs,NULL,NULL);
if(FD_ISSET(sockfd,&selFDs))
{
	buffd = skdata->data;
	len = skdata->len;
	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_control = &cmsg_b;
	msgh.msg_controllen = CMSG_LEN(len);
	iov.iov_base = "OK";
	iov.iov_len = 2;
	msgh.msg_iov = &iov;
	msgh.msg_iovlen = 1;
	cmsg = CMSG_FIRSTHDR(&msgh);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(len);
	memcpy(CMSG_DATA(cmsg), &buffd, len);
	ret = sendmsg(sockfd, &msgh, MSG_DONTWAIT);
	if (ret < 0) {
		fprintf(stderr,"ERROR: sendtosocket: sendmsg - Failed <%s>\n",strerror(errno));
		return -1;
	}
}
	return 0;
}

int receivefromsocket(int sockfd, struct socketdata *skdata)
{
	int ret = 0;
	char buf[10];
	unsigned len = 0;
	struct sockaddr_un skaddr;
	int buffd;
	char cmsg_b[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct msghdr msgh;
	struct iovec iov;
	char data[10];
	int temp = 1;
	fd_set recvFDs;


	if (!skdata) {
		fprintf(stderr,"ERROR: receivefromsocket: socketdata is NULL\n");
		return -1;
	}
	//printf("Entry -> receivefromsocket\n");
	FD_ZERO(&recvFDs);
	FD_SET(0,&recvFDs);
	FD_SET(sockfd,&recvFDs);
	ret = select(sockfd+1,&recvFDs,NULL,NULL,NULL);
if(FD_ISSET(sockfd,&recvFDs))
{
	len = sizeof(buffd);
	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_control = &cmsg_b;
	msgh.msg_controllen = CMSG_LEN(len);
	iov.iov_base = data;
	iov.iov_len = sizeof(data)-1;
	msgh.msg_iov = &iov;
	msgh.msg_iovlen = 1;
	cmsg = CMSG_FIRSTHDR(&msgh);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(len);
	ret = recvmsg(sockfd, &msgh, MSG_DONTWAIT);
	if (ret < 0) {
		fprintf(stderr,"ERROR: receivefromsocket: recvmsg - Failed <%s>\n",strerror(errno));
		return -1;
	}
	//*buffd = *((int *)CMSG_DATA(cmsg));
	memcpy(&buffd, CMSG_DATA(cmsg), len);
	//printf("receivefromsocket: len: %d, buffd: %d\n",len,buffd);
	skdata->data = buffd;
	skdata->len = len;
}
	
	return 0;
}


int closesocket(int sockfd, char *name)
{
	char sockname[20];
	close(sockfd);
	sprintf(sockname, "/tmp/%s", name);
	unlink(sockname);
	shutdown(sockfd,2);
}


