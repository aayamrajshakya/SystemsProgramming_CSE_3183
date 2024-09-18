#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#define PACKET_SIZE 20  // each packet has 20 lines/bytes of data
#define BUF_SIZE 1601 // our raw.dat is 1600 bytes

struct Packet {
    uint8_t header;   //first line is packet heading
    uint8_t flag;     // second line is flag bit and so on...
    uint8_t axL;
    uint8_t axH;
    uint8_t ayL;
    uint8_t ayH;
    uint8_t azL;
    uint8_t azH;
    uint8_t wxL;
    uint8_t wxH;
    uint8_t wyL;
    uint8_t wyH;
    uint8_t wzL;
    uint8_t wzH;
    uint8_t RollL;
    uint8_t RollH;
    uint8_t PitchL;
    uint8_t PitchH;
    uint8_t YawL;
    uint8_t YawH;
};


    // copied from Aug 29 class
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

    // function to convert raw data to useful numbers, then apply formula and display to console
void packetFunction(const struct Packet* packet) {

    // safety check
    if (packet->header != 0x55) {
        printf("invalid packet header\n");
        return;
    }

    // shifting by 8 bits and converting into signed useful integer
    int16_t ax = (packet->axH << 8) | packet->axL;
    int16_t ay = (packet->ayH << 8) | packet->ayL;
    int16_t az = (packet->azH << 8) | packet->azL;
    int16_t wx = (packet->wxH << 8) | packet->wxL;
    int16_t wy = (packet->wyH << 8) | packet->wyL;
    int16_t wz = (packet->wzH << 8) | packet->wzL;
    int16_t rollX = (packet->RollH << 8) | packet->RollL;
    int16_t pitchY = (packet->PitchH << 8) | packet->PitchL;
    int16_t yawZ = (packet->YawH << 8) | packet->YawL;

    // applying the formula
    double accelerationX = ((double)ax / 32768 * 16);
    double accelerationY = ((double)ay / 32768 * 16);
    double accelerationZ = ((double)az / 32768 * 16);

    double angularX = ((double)wx / 32768) * 2000;
    double angularY = ((double)wy / 32768) * 2000;
    double angularZ = ((double)wz / 32768) * 2000;

    double roll = ((double)rollX / 32768) * 180;
    double pitch = ((double)pitchY / 32768) * 180;
    double yaw = ((double)yawZ / 32768) * 180;

    printf("aX: %f\n", accelerationX);
    printf("aY: %f\n", accelerationY);
    printf("aZ: %f\n", accelerationZ);
    printf("wX: %f\n", angularX);
    printf("wY: %f\n", angularY);
    printf("wZ: %f\n", angularZ);
    printf("Roll: %f\n", roll);
    printf("Pitch: %f\n", pitch);
    printf("Yaw: %f\n\n", yaw);
}

int main(int argc, char *argv[]) {
    int inputFd, outputFd;
    ssize_t numRead;
    struct Packet *packet = (struct Packet *)malloc(sizeof(struct Packet));

    if (argc != 3) {
        perror("missing some arguments");
        exit(EXIT_FAILURE);
    }

    inputFd = checkError(open(argv[1], O_RDONLY), "issue with input file");
    outputFd = checkError(open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR), "issue with output file");

    int packetCount = 0;
    while ((numRead = read(inputFd, packet, PACKET_SIZE)) > 0) {
        // display the processed data in console
        printf("Packet %d:\n", ++packetCount);
        packetFunction(packet);
        
        // write processed data to outputFd (data.dat)
        checkError(write(outputFd, packet, PACKET_SIZE), "failed to write");
    }

    if (numRead == -1) {
        perror("error reading input file");
    }

    close(inputFd);
    close(outputFd);

    return EXIT_SUCCESS;
}
