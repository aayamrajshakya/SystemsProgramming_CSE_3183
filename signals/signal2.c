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

int main()
{
	int i = 0;
	for (i = 0; i < 60; i++){
		printf(".");
		sleep(i);
	}
	printf("\n");

}