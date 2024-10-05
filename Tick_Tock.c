#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>

static pid_t pPid;    //id of parent
static int tics;    //no. of tics

static void signalHandler(int sig){
    
    if (sig==SIGCHLD){
        printf("<<A child has exited!\n");
        /* we need to clean up after a child exiting
        we could call wait ....
        but wait hang...
        we will use waitpid since we can tell it not to block
        */
        while (waitpid(-1, NULL, WNOHANG) > 0) continue;
        
        exit(EXIT_SUCCESS);
    }
    if (sig == SIGUSR1){
        printf("<<Tock!\n");
        
    }
    if (sig == SIGALRM){
        printf(">>Tick! #%d\n", tics);
        kill(pPid, SIGUSR1);
        tics++;
        if (tics > 5){
            printf(">>All tic'd out, child exiting\n");
            exit(EXIT_SUCCESS);
        }
    }
    
}

int main(int argc, char * argv[]){
    
    struct sigaction sa;
    struct itimerval itmr;
    pid_t childPid = fork();
    
    tics = 0;
    itmr.it_interval.tv_sec=6;
    itmr.it_interval.tv_usec = 0;
    itmr.it_value.tv_sec = 3;
    itmr.it_value.tv_usec = 0;
    
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    
    pPid = getpid();
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction for SIGCHLD");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1){
        perror("sigaction for SIGUSR1");
        exit(EXIT_FAILURE);        
    }
    switch (childPid){
        case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        case 0:
        if (sigaction(SIGALRM, &sa, NULL) == -1){
            perror("sigaction for SIGALRM");
            exit(EXIT_FAILURE);
        }
        if (setitimer(ITIMER_REAL, &itmr, NULL) == -1){
            perror("setitimer");
            exit(EXIT_FAILURE);
        }
        while(1){
            pause();
        }
        break;
        default:
        while (1){
            pause();
        }
    }
}








