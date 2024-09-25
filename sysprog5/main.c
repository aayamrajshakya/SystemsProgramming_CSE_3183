#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>


// i checked that my longest question is under 100 characters, so this number is safe
#define TXT_LENGTH 100

// recording the status of timer
int brkTimer = 0;

int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// copied from Mr. Knight's itimer.c
void sigHandler(int sig) {
    char yorn[100];
    if (sig == SIGINT) {
        write(STDOUT_FILENO, "Exit (y/N)? ", 12);
        if (read(STDIN_FILENO, yorn, 100) == -1) {
            exit(EXIT_FAILURE);
        }
        if (yorn[0] == 'y' || yorn[0] == 'Y') {
            exit(EXIT_SUCCESS);
        }
        brkTimer = 1;
    }
    if (sig == SIGALRM) {
        write(STDOUT_FILENO, "\nTime has elapsed\n", 18);
        brkTimer = 1;
    }
}

int main() {

    // question --> stores each line of question        userAnswer --> stores user's answer from console
    // actualAnswer --> stores each line of answer      buffer --> a pointer to store each char then copy to either question or answer
    // int i --> iterating thru question char           int j --> iterating thru user's answer char

    char question[TXT_LENGTH], actualAnswer[TXT_LENGTH], userAnswer[TXT_LENGTH], proceedOrNot[5], buffer;
    int questFd, ansFd;
    int score = 0, total = 0, i = 0, j = 0;

    struct sigaction sa;
    struct itimerval it;

    // acc. to Knight, can be written as struct itimerval it = {15,0,0,0} instead of that 4 long lines (58-61)

    sa.sa_handler = sigHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    it.it_value.tv_sec = 15;
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;

    checkError(sigaction(SIGINT, &sa, NULL), "sigaction");
    checkError(sigaction(SIGALRM, &sa, NULL), "sigaction");

    printf("---Geography Quiz---\nDifficulty level: American\nThis is a timed quiz and you'll be given 15 secs for each question. You can exit the quiz by pressing ctrl+c.\n");
    printf("Do you wish to proceed (y/n)? ");
    scanf("%s", proceedOrNot);

    if (proceedOrNot[0] == 'N' || proceedOrNot[0] == 'n') {
        printf("You gave up, bye\n");
        exit(EXIT_SUCCESS);
    }

    questFd = checkError(open("quest.txt", O_RDONLY), "error opening quest.txt");
    ansFd = checkError(open("ans.txt", O_RDONLY), "error opening ans.txt");

    // acknowledgement: I found a good way to read the quest.txt and ans.txt files line by line from stackoverflow
    // link to snippet: https://stackoverflow.com/a/53687734

    while (read(questFd, &buffer, 1) == 1)
    {
        question[i++] = buffer;
        if (buffer == '\n')    // each question is in different line, this helps the program know that a whole sentence ended
        {
            question[i] = '\0';    // null terminating the question str
            i = 0;
            printf("%s", question);
            printf("answer: ");

            //starting the timer right after the prompt asks for answer
            //this makes sure the user's few seconds aren't spent showing the question
            brkTimer = 0;
            setitimer(ITIMER_REAL, &it, NULL);

            userAnswer[0] = '\0';     // nullifying the prev answer (if any) for the upcoming scanf
            scanf("%s", userAnswer);

            j = 0;
            
            // logic from stackoverflow
            while (read(ansFd, &buffer, 1) == 1)
            {
                if (buffer == '\n')
                {
                    actualAnswer[j] = '\0';     // to mark the end of the answer
                    break;
                }
                actualAnswer[j++] = buffer;     // stores all single char read by buffer into actualAnswer
            }

            // compare the user's answer with ans.txt while the timer hasn't elapsed
            while (brkTimer == 0) {
                if (strcmp(userAnswer, actualAnswer) == 0) {
                    printf("correct\n");
                    score++;
                    break;
                } else {
                    printf("wrong\n");
                    break;
                }
            }
            total++; // count the number of questions
        }
    }

    close(questFd);
    close(ansFd);

    printf("final score is %d out of %d\n", score, total);

    return 0;
}