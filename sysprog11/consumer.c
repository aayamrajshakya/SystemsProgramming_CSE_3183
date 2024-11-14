#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
	if (val == -1) {
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}
int main()
{
	int fd;
	char fifoOutput[100];

	// open the FIFO created by producer.c
	fd = open("FIFOval", O_RDONLY);
	ssize_t rdNum2 = read(fd, fifoOutput, sizeof(fifoOutput));
	checkError(rdNum2, "error reading");

	// when the prog reaches EOF, exit
	if (rdNum2 == 0) {
		exit(EXIT_FAILURE);
	}

	// null-terminating the output
	fifoOutput[rdNum2]='\0';
	printf("Consumer: %s\n", fifoOutput);
	close(fd);
	exit(EXIT_SUCCESS);
}
