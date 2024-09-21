#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>


int checkError(int val, const char *msg)
{
  if (val == -1)
  {
    perror(msg);
    exit(EXIT_FAILURE);
  }
  return val;
}


void sigHandler(int sig)
{
  char yorn[100];
  if (sig == SIGINT)
  {
    write(STDOUT_FILENO, "Do you really want to exit? (y/n): ", 35);
    if (read(STDIN_FILENO, yorn, 100) == -1) { exit(EXIT_FAILURE);}
    if (yorn[0] == 'y' || yorn[0] == 'Y') { exit(EXIT_SUCCESS);}
  }
  if (sig == SIGALRM)
  {
   write(STDOUT_FILENO, "Tic\n", 4);
  }
}

int addOne()
{
  static int one;//;

  one += 1;

  return one;
}

int main(int argc, char *argv[])
{ int j;
int val;
struct sigaction sa;
struct itimerval it;

 
 sa.sa_handler = sigHandler;
 sa.sa_flags = SA_RESTART;
 sigemptyset(&sa.sa_mask);
//sigaddset(&sa.sa_mask, SIGALRM);
 it.it_interval.tv_sec = 10;
 it.it_interval.tv_usec = 0;
 it.it_value.tv_sec = 10;
 it.it_value.tv_usec = 0;


 
  checkError(sigaction(SIGINT, &sa, NULL), "signal SIGINT");

 checkError(sigaction(SIGALRM, &sa, NULL), "signal SIGALRM");
 checkError(setitimer(ITIMER_REAL, &it, NULL), "setitimer");
 /*
for (j = 0; ; j++)
{ printf("%d\n",addOne());
  scanf("%d", &val);
 //sleep(3);
 //pause();
}
 */
 while(1)
   {
   // printf("Tic\n");
    pause();
    // sleep(3);
   }
}
