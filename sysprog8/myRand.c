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

#define TOTAL_INTEGERS 60
#define MAX_RANGE 100

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {
	int myrandFd, openFlags, num[TOTAL_INTEGERS];
	mode_t filePerms;
    srand(time(NULL));

    // set the flags and permissions for the output files
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    // guide link: https://www.iditect.com/program-example/c--how-to-generate-unique-file-names.html
    // at its longest (data255.dat), the file name is 11 chars long, so 15 is safe number
    char datFile[15];
    int X = rand() % (255 + 1);   // adding 1 to make range inclusive
    sprintf(datFile, "data%d.dat", X);
    myrandFd = checkError(open(datFile, openFlags, filePerms), "issue with opening");

    // directly copying my own code from Test1 question2
    for (int i = 0; i < TOTAL_INTEGERS; i++){
    	num[i] = rand() % (MAX_RANGE + 1);
    }
    checkError(write(myrandFd, num, sizeof(int) * TOTAL_INTEGERS), "issue with writing");
    close(myrandFd);

    // exiting with status X as mentioned in canvas
    exit(X);
}