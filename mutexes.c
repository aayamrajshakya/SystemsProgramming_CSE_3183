#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

int global;
pthread_mutex_t MTX = PTHREAD_MUTEX_INITIALIZER;

struct argz
{
    int cnt;
};

int checkThread(int val, const char *msg)
{
    if (val > 0)
    {
        errno  = val;
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void *threadStart(void *args)
{
  struct argz *argz = (struct argz *) args;
  int i = 0;
  int local = 0;
  
  for (i = 0; i < argz->cnt; i++)
  {
      checkThread(pthread_mutex_lock(&MTX),"mutex_lock");
      //local = global;
      global++;
      //global = local;
     
      checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");

  }
  pthread_exit(NULL);
}
int main()
{
    int val;
    struct argz argz;
    pthread_t thr[2];
    
    printf("Enter a count: ");
    scanf("%d",&val);
    argz.cnt = val;
    
    checkThread(pthread_create(&thr[0], NULL, threadStart,&argz), "pthread_create");
    checkThread(pthread_create(&thr[1], NULL, threadStart,&argz), "pthread_create");
    checkThread(pthread_join(thr[0],NULL),"pthread_join");
    checkThread(pthread_join(thr[1],NULL),"pthread_join");
    printf("%d\n",global);
    return 0;
}

