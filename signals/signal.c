#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int checkError(int val, const char *msg){
	if(val == -1){
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}


void sigHndlr(int sig){
	char *out = "Exit (y/n)\n";
	char yorn[5];

	if (sig == SIGINT){
		//write(STDOUT_FILENO, out, strlen(out));
		printf("%s\n", out);
		scanf("%s", yorn);
		if (yorn[0]=='Y' || yorn[0] == 'y') exit(EXIT_SUCCESS);
	

	}

}

int main()
{
	struct sigaction sa;
	
	sa.sa_handler = sigHndlr;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	checkError(sigaction(SIGINT, &sa, NULL), "sigaction");



	while (1){
		printf("Pausing for the demo\n");
		pause();
	}
	return 0;
}