#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

int global;
pthread_mutex_t MTX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CND = PTHREAD_COND_INITIALIZER;
int ready; 

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

void *consumer (void *args)
{
    double sum = 0.0;
    int count = 0;
    
    while (1)
    {
        // wait on the producer
        checkThread(pthread_mutex_lock(&MTX),"mutex_lock");
        while (!ready)
        {
            checkThread(pthread_cond_wait(&CND,&MTX),"cond_wait");
        }
        ready =0;
        if (global == 0) break;
        sum += global;
        count++;
        printf("%d values generated\nCurrent average %lf\n\n",count,(sum/count));
        checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
    }
    
    pthread_exit(NULL);
}

void *producer (void *args)
{
    struct timespec ts = {0,0};
    srand(time(NULL));
    checkThread(pthread_detach(pthread_self()), "pthread_detach");
    while(1)
    {
     checkThread(pthread_mutex_lock(&MTX),"mutex_lock");
     global = rand() % 101;
     ts.tv_sec= rand() % 5;
     ready = 1;
     checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
     checkThread(pthread_cond_signal(&CND),"cond_signal");
     if (global == 0) break;
     nanosleep(&ts,NULL);
    }
    
    pthread_exit(NULL);
}




int main()
{
    int val;
    pthread_t thr[2];
    
    
    checkThread(pthread_create(&thr[0], NULL, consumer,NULL), "pthread_create");
    checkThread(pthread_create(&thr[1], NULL, producer,NULL), "pthread_create");
    checkThread(pthread_join(thr[0],NULL),"pthread_join");
    printf("Exiting\n");
    return 0;
}


