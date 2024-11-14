#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include "semun.h"
#include "binary_sem.h"

#define shm_size 1024

struct block {
    int length;
    char character;
};

struct shared_data {
    int total_blocks;
    struct block blocks[20];
};


// Mr. Knight's random number generator
int genRand(int low, int high) {
    int rng = high - low + 1;
    double drnd = rand();
    int irnd = drnd / ((double)RAND_MAX + 1) * rng;
    return low + irnd;
}

// Mr. Knight's error checker
int checkError(int e, const char *str)
{
  if (e == -1)
    {
      if (errno == EINTR) return -1;
      perror(str);
      exit(EXIT_FAILURE);
    }
  return e;
}

// Child process
void childProcess(int sem_id, int shm_id) {
    // attach segment of shared memory
    struct shared_data *data = shmat(shm_id, NULL, 0);
    if (data == (void *)-1) { checkError(-1, "shmat");}

    srand(time(NULL));
    
    // reserve child semaphore
    checkError(reserveSem(sem_id, 0), "reserveSem");

    // using genRand to generate and store total number of blocks to generate
    data->total_blocks = genRand(10, 20);

    // generate and store length and character for each block
    for (int i = 0; i < data->total_blocks; i++) {
        data->blocks[i].length = genRand(2, 10);
        data->blocks[i].character = 'a' + (genRand(0, 25));
    }

    // release parent semaphore
    checkError(releaseSem(sem_id, 1), "releaseSem");

    // reserve child semaphore
    checkError(reserveSem(sem_id, 0), "reserveSem");

    // detach the shared memory segment
    checkError(shmdt(data), "shmdt");

    // release parent semaphore
    checkError(releaseSem(sem_id, 1), "releaseSem");
}

void parentProcess(int sem_id, int shm_id) {
    // attach segment of shared memory
    struct shared_data *data = shmat(shm_id, NULL, 0);
    if (data == (void *)-1) { checkError(-1, "shmat");}

    srand(time(NULL));

    // reserve parent semaphore
    checkError(reserveSem(sem_id, 1), "reserveSem");

    // generate width for ASCII art between 10 and 15
    int width = genRand(10, 15);
    int current_width = 0;

    // display the generated character-length pairs
    // couldn't think of any more efficient way to do this, so i made the same for loop twice
    printf("Generating the pairs: ");
    for (int i = 0; i < data->total_blocks; i++) {
        printf("(%c,%d) ", data->blocks[i].character, data->blocks[i].length);
    }
    printf("\n");

    // use the data stored in shared memory to generate text art
    for (int i = 0; i < data->total_blocks; i++) {
        for (int j = 0; j < data->blocks[i].length; j++) {
            printf("%c", data->blocks[i].character);
            current_width++;
            if (current_width == width) {
                printf("\n");
                current_width = 0;
            }
        }
    }
    if (current_width > 0) {
        printf("\n");

    }

    // release child semaphore
    checkError(releaseSem(sem_id, 0), "releaseSem");

    // reserve parent semaphore
    checkError(reserveSem(sem_id, 1), "reserveSem");

    // detach the shared memory segment
    checkError(shmdt(data), "shmdt");
}

int main() {
    int sem_id, shm_id;

    // create semaphore set of size 2 with IPC_PRIVATE as key
    checkError(sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666), "semget");

    // initialize child semaphore to "available"
    checkError(initSemAvailable(sem_id, 0), "initSemAvailable");

    // initialize parent semaphore to "in use"
    checkError(initSemInUse(sem_id, 1), "initSemInUse");

    // create a segment of shared memory with IPC_PRIVATE as key
    checkError(shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666), "shmget");
 
    pid_t pid = checkError(fork(), "fork fail");
    if (pid == 0) {
        childProcess(sem_id, shm_id);
        exit(0);
    } else {
        parentProcess(sem_id, shm_id);
        wait(NULL);
    }

    // delete the semaphore and shared memory
    checkError(semctl(sem_id, 0, IPC_RMID), "semctl");
    checkError(shmctl(shm_id, 0, IPC_RMID), "shmctl");
    exit(EXIT_SUCCESS);
}