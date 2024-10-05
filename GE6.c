#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void child(int i){
    
    pid_t pid=getpid();
    printf("Starting child %d", i);
    
    printf(": pid = %d\n", pid);
    
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    pid_t childId[2];
    pid_t pid;
    int i = 0;
    
    pid = getpid();
    
    printf("parent pid = %d\n", pid);
    
    for (i=0; i<2; i++){
        childId[i] = fork();
        if (childId[i] == 0){ child(i+1); }
        if (childId[i] == -1) { perror("fork"); exit(EXIT_FAILURE);}
    }
    sleep(5);
    exit(EXIT_SUCCESS);
}


