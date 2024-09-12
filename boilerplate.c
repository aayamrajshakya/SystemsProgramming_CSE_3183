#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // Debug output to check the number of arguments
    printf("argc = %d\n", argc);

    // Check for the correct number of arguments
    if (argc != 3) {
        perror("too few args");
        exit(EXIT_FAILURE);
    }

    // Open the input file in read-only mode
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        perror("open for read");
        exit(EXIT_FAILURE);
    }

    // Set the flags and permissions for the output file
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    // Open the output file with the specified flags and permissions
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        perror("open for write");
        exit(EXIT_FAILURE);
    }

    // Read from the input file and write to the output file
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    // Check for read errors
    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Close the input and output files
    close(inputFd);
    close(outputFd);

    // Exit successfully
    exit(EXIT_SUCCESS);
}
