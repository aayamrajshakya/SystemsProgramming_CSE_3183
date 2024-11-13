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
      if (errno == EINTR) return -1;
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


  fd = checkError(open("synShm",O_RDONLY), "open for sync");
  close(fd);
  
  semid = checkError(semget(semK,0,0),"semid");
  shmid = checkError(shmget(shmK,0,0),"shmid");

  smap = shmat(shmid, NULL, SHM_RDONLY);
  if (smap == (void *) -1)
    {
      checkError(-1, "shmat");
    }

  checkError(reserveSem(semid,READER),"reserveSem");

  total = 0;

  printf("Reading %d values\n",smap->cnt);
  for (i = 0; i < smap->cnt; i++)
    {

      total += smap->vals[i];
    }

  printf("They total %d\n",total);

  checkError(releaseSem(semid,WRITER),"releaseSem");

  checkError(shmdt(smap),"shmdt");

  exit(EXIT_SUCCESS);
}
