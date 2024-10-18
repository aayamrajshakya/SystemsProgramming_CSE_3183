#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define TOTAL_INTEGERS 60

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main(){
	pid_t pid;
	int myrandFd, status, X, num[TOTAL_INTEGERS];
	float sum, avg;  // average can consist decimals, so using float
	ssize_t numRead;
	char *file = "myRand";

	pid = checkError(fork(), "fork fail");
	// running the execl command when child is formed
	if (pid == 0){
		checkError(execl(file, file, NULL), "issue with execl");
	} 
	// waiting for child to exit
	waitpid(pid, &status, 0);

	// this runs if the child exited normally
	if (WIFEXITED(status)){
		X = WEXITSTATUS(status);    // this is the macro that retuns the exit status of the child
									// which ranges from 0-255 (canvas slide 14: Processes, page 8)

		// almost same as the myRand.c
		char datFile[15];    
		sprintf(datFile, "data%d.dat", X);   // X is the number returned from exit status of child
		myrandFd = checkError(open(datFile, O_RDONLY), "issue with opening");

		numRead = checkError(read(myrandFd, num, sizeof(int) * TOTAL_INTEGERS), "issue with reading");
		close(myrandFd);

		// displaying file numbers and calculating their avg
		printf("numbers in the file:\n");
		for (int i = 0; i < TOTAL_INTEGERS; i++){
			printf("%d\n", num[i]);
			sum += num[i];
		}
		avg = sum / TOTAL_INTEGERS;
		printf("the average is: %f\n", avg);

		// unlink deletes the datFile after the program, that's why you won't see the file
		unlink(datFile);
	}
exit(EXIT_SUCCESS);
	
}
