
#ifndef _IPCSOCKET_H
#define _IPCSOCKET_H



struct socketdata {
	int data;
	unsigned int len;
};

int opensocket(int *sockfd, const char *name, int connecttype);
int sendtosocket(int sockfd, struct socketdata *data);
int receivefromsocket(int sockfd, struct socketdata *data);
int closesocket(int sockfd, char *name);


#endif

