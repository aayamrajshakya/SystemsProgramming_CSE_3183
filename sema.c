#include "binary.h"
#include "semun.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define READER 0
#define WRITER 1

struct shmSeg {
    int n;
    int valz[1000];
};

int checkError(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void child(key_t semK, key_t shmK, int fd[]) {
    int semId, shmId;
    struct shmSeg *addr;

    srand(time(NULL));

    close(fd[READER]);

    semId = checkError(semget(semK, 2, IPC_CREAT | S_IRUSR | S_IWUSR), "semget");
    // initialize them
    initSemAvailable(semId, WRITER);
    initSemInUse(semId, READER);

    shmId = checkError(shmget(shmK, sizeof(struct shmSeg), IPC_CREAT, S_IRUSR | S_IWUSR), "shmget");

    close(fd[WRITER]);

    checkError(reserveSem(semId, WRITER), "reserve the writer");
    addr = (struct shmSeg *) shmat(shmId, NULL, 0);

    addr->n = rand() % 900 + 1;
    int i = 0;

    for (i = 0; i < addr->n; i++) {
        addr->valz[i] = rand() % 1000 + 1;
    }

    checkError(releaseSem(semId, READER), "release the reader");
    checkError(reserveSem(semId, WRITER), "reserve the writer");

    shmdt(addr);
    checkError(shmctl(shmId, IPC_RMID, NULL), "delete the shared memory");
    checkError(semctl(semId, 0, IPC_RMID), "delete the semaphore set");

    exit(EXIT_SUCCESS);
}

void parent(key_t semK, key_t shmK, int fd[]) {
    int semId, shmId;
    struct shmSeg *addr;
    int i = 0;
    int sum = 0;
    char garbage;

    srand(time(NULL));

    close(fd[WRITER]);
    // block for synchronization
    checkError(read(fd[READER], &garbage, 1), "read");

    semId = checkError(semget(semK, 0, 0), "semget");
    shmId = checkError(shmget(shmK, 0, 0), "shmget");

    checkError(reserveSem(semId, READER), "reserve the reader");
    addr = (struct shmSeg *) shmat(shmId, NULL, 0);

    printf("Reading %d values\n", addr->n);

    for (i = 0; i < addr->n; i++) {
        sum += addr->valz[i];
    }

    printf("The sum of those values were %d\n", sum);
    checkError(reserveSem(semId, WRITER), "reserve the writer");

    shmdt(addr);
    exit(EXIT_SUCCESS);
}

int main() {
    key_t semK, shmK;
    pid_t pid;
    int fd[2];

    checkError(pipe(fd), "pipe");

    semK = ftok("./a.out", 1000);
    shmK = ftok("./a.out", 2000);

    pid = checkError(fork(), "fork");

    if (pid == 0) {
        child(semK, shmK, fd);
    }

    parent(semK, shmK, fd);

    return 0;
}


/*int bsRetryOnEintr = 1;
int bsUseSemUndo = 0;

int initSemAvailable(int semId, int semNum)
{
    union semun arg;
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

int initSemInUse(int semId, int semNum)
{
    union semun arg;
    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}

int reserveSem(int semId, int semNum) {
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    while (semop(semId, &sops, 1) == -1) {
        if (errno != EINTR || !bsRetryOnEintr)
            return -1;
    }
    return 0;
}

int releaseSem(int semId, int semNum) {
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    return semop(semId, &sops, 1);
}*/

