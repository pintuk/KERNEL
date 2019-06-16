#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include "crashgen.h"

#define CRASH_DEVICE	"/dev/crashgen"

int main(int argc, char *argv[])
{
	int fd;
	int ret;

	fd = open(CRASH_DEVICE, O_RDWR);
	if (fd < 0) {
		perror("::ERROR - opening device : ");
		exit (-1);
	}
	ret = ioctl(fd, CRASH_IOC_PTR_NONE, 0);
	if (ret < 0) {
		printf("ERROR : ioctl - CRASHGEN_IOC_PTR_NONE - Failed\n");
		return -1;
	}
	printf("DONE...\n");
	close(fd);

	return 0;
}

