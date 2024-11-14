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
	char fifoInput[100];

	// create a FIFO to be used by consumer.c
	if (mkfifo("FIFOval", S_IRWXU) == -1)
	{
		if (errno != EEXIST)
		{
			perror("mkfifo");
			exit(EXIT_FAILURE);
		}
	}


	// open the FIFO to write the user input into it
	fd = open("FIFOval", O_WRONLY);
	char *prompt = "Please enter text at the producer:\nProducer: ";
	write(STDOUT_FILENO, prompt, strlen(prompt));
	ssize_t rdNum1 = read(STDIN_FILENO, fifoInput, sizeof(fifoInput));
	checkError(rdNum1, "error reading");

	// when the prog reaches EOF, exit
	if (rdNum1 == 0) {
		exit(EXIT_FAILURE);
	}
	// null-terminating the input to avoid weird symbols
	fifoInput[rdNum1-1]='\0';
	checkError(write(fd, fifoInput, rdNum1), "writing failed");
	close(fd);
	exit(EXIT_SUCCESS);
}
