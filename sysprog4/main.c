#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

#define BUF_SIZE 12  //960 bytes, 960/80 = 12 bytes/packet = 3 float * 4 bytes each

// int brkPause = 0;

int checkError(int val, const char* msg) {
	if (val == -1) {
		perror(msg);
		exit(EXIT_FAILURE);
}
	return val;
}

void sigHndlr(int sig){
	char *msg = "Exit (y/N)?";
	char yorn[5];
	int rdNum;
	if (sig==SIGINT)
	{
		write(STDOUT_FILENO, msg, strlen(msg));
		rdNum = read(STDIN_FILENO, yorn, 5);
		if (rdNum == 2 && (yorn[0] == 'Y' || yorn[0] == 'y')) exit(EXIT_SUCCESS);
		return ;
	}
	/*if (sig == SIGALRM)
	{
		brkPause = 1;
	} */
}

int main()
{
	int anglFd;
	ssize_t numRead;
	float buffer[3];
	
	anglFd = checkError(open("angl.dat", O_RDONLY), "issue with the .dat file");

	struct sigaction sa;
	// struct itimerval it = {0,0,0,0};
	struct timespec ts = {1,0};  // interval of 1 second as mentioned in rubric

	sa.sa_handler = sigHndlr;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	checkError(sigaction(SIGINT, &sa, NULL), "sigaction");
	// checkError(sigaction(SIGALRM, &sa, NULL), "sigaction");
	// checkError(setitimer(ITIMER_REAL, &it, NULL), "setitimer");

	printf("Reading the angl.dat file now\n");
	printf("Displaying packet data w/ roll and pitch values in range (-20,20).\n\n");

	int packetIndex = 0;
	while(1){
		nanosleep(&ts, NULL);
		numRead = read(anglFd, buffer, BUF_SIZE);
		checkError(numRead, "error reading the packet");
		if (numRead == 0) break; // when the prog reaches EOF, exit the loop

		float roll = buffer[0];
		float pitch = buffer[1];
		float yaw = buffer[2];

		printf("Packet no. %d-> ", packetIndex + 1);
        if ((roll < -20 || roll > 20) || (pitch < -20 || pitch > 20)){
            if (roll < -20 || roll > 20){
                printf("Roll is out of range. ");
            }
            if (pitch < -20 || pitch > 20){
                printf("Pitch is out of range. ");
            }
            printf("\n\n");
        }
        else{
            printf("Roll: %f, Pitch: %f, Yaw: %f\n\n", roll, pitch, yaw);
        }
        packetIndex++;
    }
	printf("done reading...\n");
	close(anglFd);

	return 0;
}
