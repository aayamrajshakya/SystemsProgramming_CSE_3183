#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "semun.h"
#include "binary_sem.h"

#define READER 1
#define WRITER 0
#define BUF_SIZE 1024
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct shmseg
{
  int cnt;
  int vals[BUF_SIZE];
};

int genRand(int low, int high)
{
  int rng = high - low + 1;
  double drnd = rand();
  int irnd = drnd/((double) RAND_MAX + 1) * rng;
  return low + irnd;
}

int checkError(int e, const char *str)
{
  if (e == -1)
    {
      if (e == EINTR) return -1;
      perror(str);
      exit(EXIT_FAILURE);
    }
  return e;
}


int main(int argc, char *argv[])
{
  
  struct shmseg *smap;
  int cnt;
  int i = 0;
  int total = 0;
  int fd;  
  int semid, shmid, val;
  union semun dummy;
  
  key_t semK, shmK;


  shmK = ftok(".",2000);
  semK = ftok(".",4500);

  srand(time(NULL));

  cnt = genRand(1, BUF_SIZE);

  semid = checkError(semget(semK, 2, IPC_CREAT | OBJ_PERMS), "semget");

  checkError(initSemAvailable(semid,WRITER), "initSemAvailable");
  checkError(initSemInUse(semid,READER),"initSemInUse");

  shmid = checkError(shmget(shmK, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS),"shmget");

  fd = checkError(open("synShm",O_WRONLY),"open for sync");
  close(fd);
  
  smap = shmat(shmid, NULL, 0);
  if (smap == (void *) -1 )
    {
      checkError(-1, "shmat");
    }

  checkError(reserveSem(semid,WRITER),"reserveSem");

  smap->cnt = cnt;

  printf("Creating %d values\n",cnt);
  total = 0;
  for (i = 0; i < cnt; i++)
    {
      val = genRand(1,1000);
      smap->vals[i] = val;
      total += val;
    }

  printf("They totaled %d\n",total);

  checkError(releaseSem(semid,READER),"releaseSem");

  checkError(reserveSem(semid,WRITER),"reserveSem");

  checkError(shmdt(smap),"shmdt");

  checkError(semctl(semid,0,IPC_RMID),"semctl");
  checkError(shmctl(shmid,0,IPC_RMID),"shmctl");

  exit(EXIT_SUCCESS);
}
