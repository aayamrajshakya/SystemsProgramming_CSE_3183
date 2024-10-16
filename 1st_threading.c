#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <string.h>


struct arg
{
    int x;
};


int checkError(int val, const char *msg)
{
    if (val == -1)
    {
        if (errno == EINTR) return -1;
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int threadError(int val, const char *msg)
{
    if (val == 0) return 0;
    errno = val;
    perror(msg);
    exit(EXIT_FAILURE);
}

void *startThread(void *args)
{
    struct arg *arg = (struct arg *) args;
    struct timespec ts = {5,0};
    printf("Starting the thread %d\n",arg->x);
    nanosleep(&ts,NULL);
    printf("Exiting the thread %d\n",arg->x);
    pthread_exit(NULL);
}

// gcc main.c -pthread
int main()
{
    struct arg v[3];
    void *vptr;
    pthread_t thr[3];
    int i = 0;
    
    for (i = 0; i < 3; i++)
    {
    printf("Creating thread %d\n",i+1);
    v[i].x = i+1;
    vptr = (void *) &v[i];
    threadError(pthread_create(&thr[i],NULL,startThread,vptr),"pthread_create");
    }
    for (i = 0; i < 3; i++)
    {
    printf("Now I will join the thread %d\n",i+1);
    threadError(pthread_join(thr[i],NULL),"pthread_join");
    }
    printf("The threads are over");

    return 0;
}

