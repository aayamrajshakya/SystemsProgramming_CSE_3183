#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

int brkPause = 0;

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
	if (sig == SIGALRM)
	{
		brkPause = 1;
	}
}

int main(int argc, char *argv[])
{
	struct sigaction sa;
	struct itimerval it;     // can do "....it = {0,0,10,0};"
	struct timespec ts;


	ts.tv_sec = 5;
	ts.tv_nsec = 0;

	sa.sa_handler = sigHndlr;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;

	// can commented the 4 'it' and pass struct itimerval it = {0,0,10,0}

	checkError(sigaction(SIGINT, &sa, NULL), "sigaction");
	checkError(sigaction(SIGALRM, &sa, NULL), "sigaction");
	checkError(setitimer(ITIMER_REAL, &it, NULL), "setitimer");

	printf("We are pausing for a moment\n");
	while(1){
		// pause();
		nanosleep(&ts, NULL);
		printf("%d.....\n", ++brkPause);
		if (brkPause > 5) break;
	}
	printf("done pausing....\n");


	return 0;
}
