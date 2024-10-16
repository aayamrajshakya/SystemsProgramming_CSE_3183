#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

struct argz
{
    int which;
};


int global[20];
pthread_mutex_t MTX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CND = PTHREAD_COND_INITIALIZER;
pthread_mutex_t MTXw = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CNDw = PTHREAD_COND_INITIALIZER;
int ready[2]; 
int awake;

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
    struct argz * argz = (struct argz *) args;
    
    printf("c%d\n",argz->which);
    
    checkThread(pthread_mutex_lock(&MTXw),"mutex_lock");
    awake++;
    checkThread(pthread_mutex_unlock(&MTXw),"mutex_unlock");
    checkThread(pthread_cond_broadcast(&CNDw),"cond_signal");
    
    
    while (1)
    {
        // wait on the producer
        checkThread(pthread_mutex_lock(&MTX),"mutex_lock");
        while (!ready[argz->which])
        {
            checkThread(pthread_cond_wait(&CND,&MTX),"cond_wait");
        }
        while (ready[argz->which])
        {
            ready[argz->which]--;
            if (global[ready[argz->which]] == 0) 
            {
                checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
                pthread_exit(NULL);
            }
            sum += ((argz->which == 1) ? -1 : 1 ) * global[ready[argz->which]];  
            count++;
        }
        ready[argz->which] = 0;
        printf("Consumer %d: %d values generated\nCurrent average %lf\n\n",argz->which, count,(sum/count));
        checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
    }

    pthread_exit(NULL);
}

void *producer (void *args)
{
    struct timespec ts = {0,0};
    srand(time(NULL));
    printf("p\n");
    checkThread(pthread_detach(pthread_self()), "pthread_detach");
    
    checkThread(pthread_mutex_lock(&MTXw),"mutex_lock");
    while (awake < 2)
    {
        checkThread(pthread_cond_wait(&CNDw,&MTXw),"cond_wait");
       // printf("%d\n",awake);
    }
    checkThread(pthread_mutex_unlock(&MTXw),"mutex_unlock");
    
    while(1)
    {
        int valz = rand() % 16 + 5;
        int i = 0;
     checkThread(pthread_mutex_lock(&MTX),"mutex_lock");
     for (i = 0; i < valz; i++)
     {
        global[i] = rand() % 101; 
        if (global[i] == 0) 
        {
            ready[0] = ready[1] = valz;
            checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
            checkThread(pthread_cond_broadcast(&CND),"cond_signal");
            pthread_exit(NULL);
        }
     }

     ts.tv_sec= rand() % 5 + 1;
     ready[0] = ready[1] = valz;
     checkThread(pthread_mutex_unlock(&MTX),"mutex_unlock");
     checkThread(pthread_cond_broadcast(&CND),"cond_signal");

     nanosleep(&ts,NULL);
    }
    
    pthread_exit(NULL);
}




int main()
{
    int val;
    pthread_t thr[3];
    
    struct argz argz[2] = {0,1};
    checkThread(pthread_create(&thr[0], NULL, consumer,(void *) &argz[0]), "pthread_create");
    checkThread(pthread_create(&thr[1], NULL, consumer,(void *) &argz[1]), "pthread_create");
    checkThread(pthread_create(&thr[2], NULL, producer,NULL), "pthread_create");
    checkThread(pthread_join(thr[0],NULL),"pthread_join");
    checkThread(pthread_join(thr[1],NULL),"pthread_join");
    printf("Exiting\n");
    return 0;
}





