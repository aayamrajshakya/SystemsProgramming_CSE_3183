#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 12

pid_t pid;

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

//pattern from Mr. Knight's waitpid.c
void exitP(void)
{
    char *out1 = "Exiting the parent...\n";
    char *out2 = "Parent has normally terminated\n";
    write(STDOUT_FILENO, out1, strlen(out1));

    // kills child process if any
    if (pid > 0) {
        kill(pid, SIGTERM);
    }
    write(STDOUT_FILENO, out2, strlen(out2));
}

void exitC(void)
{
    char *out = "Child has normally terminated\n";
    write(STDOUT_FILENO, out, strlen(out));
}

void signalHandlerP(int sig) {
    pid_t p;
    int status;

    //directly copied from Mr. Knight's waitpid.c
    if (sig == SIGCHLD) {
        while ((p = waitpid(-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED(status)) {
                printf("Child @ pid %d is exiting\n", p);
            }
        }
        if (p == -1)
        {
            if (errno == ECHILD)
            {
                exit(EXIT_SUCCESS);
            }
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }


    if (sig == SIGINT)
    {
        char yorn[5];
        ssize_t bytesRead;
        write(STDOUT_FILENO, "Exit: Are you sure (Y/n)? ", 27);
        bytesRead = checkError(read(STDIN_FILENO, yorn, sizeof(yorn)), "signit input read error");

        //if user enters 'y', close any child process and then exit the program
        if (bytesRead > 0 && (yorn[0] == 'y' || yorn[0] == 'Y')) {
            if(pid > 0) {
                kill(pid, SIGTERM);
            }
            exit(EXIT_SUCCESS);
        }
    }

    if (sig == SIGUSR1) {
        printf("Warning! roll outside of bounds\n");
    }

    if (sig == SIGUSR2) {
        printf("Warning! pitch outside of bounds\n");
    }
}

void signalHandlerC(int sig) {
    if (sig == SIGTERM) {
        printf("The parent has died. Child needs to terminate too\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    int anglFd;
    struct sigaction saP, saC;

    saP.sa_handler = signalHandlerP;
    sigemptyset(&saP.sa_mask);
    saP.sa_flags = SA_RESTART;

    checkError(sigaction(SIGCHLD, &saP, NULL), "SIGCHLD issue");
    checkError(sigaction(SIGINT, &saP, NULL), "SIGINT issue");
    checkError(sigaction(SIGUSR1, &saP, NULL), "SIGUSR1 issue");
    checkError(sigaction(SIGUSR2, &saP, NULL), "SIGUSR2 issue");

    saC.sa_handler = signalHandlerC;
    sigemptyset(&saC.sa_mask);
    saC.sa_flags = 0;

    printf("Creating parent\n");
    printf("Executing parent\n\n");

    pid = checkError(fork(), "fork fail");
    if (pid == 0) {
        printf("Creating child\n");
        printf("Executing child\n\n");

        atexit(exitC);

        //blocking SIGINT in child to prevent killing both parent and child
        //https://stackoverflow.com/a/54353356
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        checkError(sigprocmask(SIG_BLOCK, &mask, NULL), "sigprocmask issue");

        checkError(sigaction(SIGTERM, &saC, NULL), "sigaction issue");

        anglFd = checkError(open("angl.dat", O_RDONLY), "issue with the .dat file");

        float buffer[3];
        struct timespec ts = {1,0};  // interval of 1 second as mentioned in rubric

        while (1) {
            nanosleep(&ts, NULL);
            ssize_t numRead = read(anglFd, buffer, BUF_SIZE);
            checkError(numRead, "error reading the packet");
            if (numRead == 0) break; // when the prog reaches EOF, exit the loop

            float roll = buffer[0];
            float pitch = buffer[1];
            float yaw = buffer[2];

            if (roll < -20.0 || roll > 20.0) {
                //sending a signal to SIGUSR1 if the above comparison is true
                //this doesn't do anything but send signal to SIGUSR1,
                //which in turn prints the 'out of range' message
                checkError(kill(getppid(), SIGUSR1), "process doesn't exist");
            }
            if (pitch < -20.0 || pitch > 20.0) {
                //same explanation as above
                checkError(kill(getppid(), SIGUSR2), "process doesn't exist");
            }
        }
        close(anglFd);
        exit(EXIT_SUCCESS);
    } else {
        atexit(exitP);
        while (1) {
            pause();
        }
    }
    exit(EXIT_SUCCESS);
}
