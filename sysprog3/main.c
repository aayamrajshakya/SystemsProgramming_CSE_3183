#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <dirent.h>

#define PACKET_SIZE 80 // we have 80 packets in data.dat 
#define BUF_SIZE 36 //data.dat is 2880 bytes, 2880 bytes /80 packets = 36 bytes/packet

// copied from Aug 29 class
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {
    int inputFd, acclFd, anglFd, rotaFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    float *buffer = (float *)malloc(BUF_SIZE);

    if (buffer == NULL){
        perror("malloc issue");
        exit(EXIT_FAILURE);
    }

    // set the flags and permissions for the output files
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    // create a directory called "values"
    checkError(mkdir("values", S_IRWXU), "can't create the directory");

    // create 3 different .dat files inside the "values" dir
    acclFd = checkError(open("values/accl.dat", openFlags, filePerms), "issue with accl.dat file");
    anglFd = checkError(open("values/angl.dat", openFlags, filePerms), "issue with angl.dat file");
    rotaFd = checkError(open("values/rota.dat", openFlags, filePerms), "issue with rota.dat file");

    // open the data.dat for reading
    inputFd = checkError(open("data.dat", O_RDONLY), "issue with input data file");  

    // iterate thru the packets in data.dat
    for (int i = 0; i < PACKET_SIZE; i++) {
        // read up to the end of each packet
        numRead = read(inputFd, buffer, BUF_SIZE);  
        checkError(numRead, "error reading packet data");

        // write accX, accY, accZ to accl.dat
        // our data.dat has no header and flag, so the first elem is accelerationX

        checkError(write(acclFd, &buffer[0], sizeof(float)), "error with accX");
        checkError(write(acclFd, &buffer[1], sizeof(float)), "error with accY");
        checkError(write(acclFd, &buffer[2], sizeof(float)), "error with accZ");

        // write wx, wy, wz to angl.dat
        checkError(write(anglFd, &buffer[3], sizeof(float)), "error with wx");
        checkError(write(anglFd, &buffer[4], sizeof(float)), "error with wy");
        checkError(write(anglFd, &buffer[5], sizeof(float)), "error with wz");

        // write roll, pitch, yaw to rota.dat
        checkError(write(rotaFd, &buffer[6], sizeof(float)), "error with roll");
        checkError(write(rotaFd, &buffer[7], sizeof(float)), "error with pitch");
        checkError(write(rotaFd, &buffer[8], sizeof(float)), "error with yaw");
    }

    close(inputFd);
    close(acclFd);
    close(anglFd);
    close(rotaFd);
    free(buffer);

    printf("Project 3 completed. Check the `values` dir\n");
    exit(EXIT_SUCCESS);
}