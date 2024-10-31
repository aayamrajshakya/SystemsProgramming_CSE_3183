// Reference from --> https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main(){
	//fd[0] will be file descriptor for the read end of pipe and fd[1] is for write end of pipe
	int fd[2];         
	char pipeInput[1024];
	pid_t p;

	// creating pipe
	checkError(pipe(fd), "pipe failed");

	// creating child
	p = checkError(fork(), "fork failed");

	// parent process
	if (p > 0){
		close(fd[0]);    // close reading end of the pipe

		// parent prompts user to enter words
		char *prompt = "Please enter text at the parent:\nParent: ";
		write(STDOUT_FILENO, prompt, strlen(prompt));
		ssize_t rdNum1 = read(STDIN_FILENO, pipeInput, sizeof(pipeInput));    // read user's input from console
		checkError(rdNum1, "error reading");
		if (rdNum1 == 0){
			exit(EXIT_FAILURE);    // when the prog reaches EOF, exit
		}
		pipeInput[rdNum1-1]='\0'; //null terminate to prevent weird symbols
		checkError(write(fd[1], pipeInput, rdNum1), "writing failed");    // write user input to writing end of the pipe	
		close(fd[1]);						                              // and close that pipe	
		wait(NULL);
	}
	// child process
	else{
		close(fd[1]);    // close writing end of pipe
		char pipeOut[1024];
		ssize_t rdNum2 = read(fd[0], pipeOut, sizeof(pipeOut));   // read data from reading end of pipe
		checkError(rdNum2, "error reading");
		if (rdNum2 == 0){
			exit(EXIT_FAILURE);    // when the prog reaches EOF, exit
		} 
		pipeOut[rdNum2]='\0';
		printf("Child: %s", pipeOut);			 // print out the read data as mentioned in Canvas
		close(fd[0]);                            // close reading end of pipe
		exit(EXIT_SUCCESS);
	}
}