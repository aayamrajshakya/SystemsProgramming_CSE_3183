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

static int SIGUSR1_signal = 0, SIGUSR2_signal = 0, SIGINT_signal = 0;
int player1_score = 0, player2_score = 0;
pid_t child1, child2;

// copied from previous programs
int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// Parent signal handler
void signalHandlerP(int sig) {
    if (sig == SIGUSR1) {
        SIGUSR1_signal = 1;
    } else if (sig == SIGUSR2) {
        SIGUSR2_signal = 1;
    } else if (sig == SIGINT) {
        // SIGINT should send SIGTERM to both children
        kill(child1, SIGTERM);
        kill(child2, SIGTERM);
        exit(EXIT_SUCCESS);
    } else if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);   // reaping child processes
    }  
}

// Children signal handler
void signalHandlerC(int sig) {
    if (sig == SIGUSR1) {
        SIGUSR1_signal = 1;             // low guess
    } else if (sig == SIGUSR2) {
        SIGUSR2_signal = 1;             // high guess
    } else if (sig == SIGINT) {
        SIGINT_signal = 1;
    } else if (sig == SIGTERM) {
        printf("exiting the child\n");
        exit(EXIT_SUCCESS);             // SIGTERM exits the child  
    }
}

void player1() {
    // setting dispositions for player1
    signal(SIGUSR1, signalHandlerC);
    signal(SIGUSR2, signalHandlerC);
    signal(SIGINT, signalHandlerC);
    signal(SIGTERM, signalHandlerC);
    struct timespec ts = {1,0};

    // waiting until the parent has sent SIGUSR1 or SIGUSR2
    while (!SIGUSR1_signal && !SIGUSR2_signal) {
        pause();
    }  

    while (1) {
        int min = 0, max = 101, guess;
        kill(getppid(), SIGUSR1);

        while (1) {
            SIGUSR1_signal = SIGUSR2_signal = SIGINT_signal = 0;
            // Player1 computes guess by taking avg of min and max
            guess = (min+max)/2;

            // creating and writing guesses on player1's txt file
            int player1Fd = checkError(open("player1.txt", openFlags, filePerms), "issue with opening");
            checkError(write(player1Fd, &guess, sizeof(int)), "issue with writing");
            close(player1Fd);

            // sleep for 1 sec and then signal the parent
            nanosleep(&ts, NULL);
            kill(getppid(), SIGUSR1);

            // wait until the parent responds with a signal
            while (!SIGUSR1_signal && !SIGUSR2_signal && !SIGINT_signal) {
                pause();
            } 

            if (SIGUSR1_signal) {  // guess was low
                min = guess;
            }
            if (SIGUSR2_signal) {  // guess was high
                max = guess;
            }
            if (SIGINT_signal) {   // guess was correct, so exiting the loop to begin new
                break;
            }
        }
    }
}        

void player2() {
    // setting dispositions for player2
    signal(SIGUSR1, signalHandlerC);
    signal(SIGUSR2, signalHandlerC);
    signal(SIGINT, signalHandlerC);
    signal(SIGTERM, signalHandlerC);
    struct timespec ts = {1,0};

    // need this because player2 is having random guesses unlike player1's fixed average everytime
    srand(time(NULL));
    // waiting until the parent has sent SIGUSR1 or SIGUSR2
    while (!SIGUSR1_signal && !SIGUSR2_signal) {
        pause();
    }  

    while (1) {
        int min = 0, max = 101, guess;
        kill(getppid(), SIGUSR2);

        while (1) {
            SIGUSR1_signal = SIGUSR2_signal = SIGINT_signal = 0;
            // player2 selects random num between min and max
            guess = (rand() % (max - min)) + min;

            // creating and writing guesses on player2's txt file
            int player2Fd = checkError(open("player2.txt", openFlags, filePerms), "issue with opening");
            checkError(write(player2Fd, &guess, sizeof(int)), "issue with writing");
            close(player2Fd);

            // sleep for 1 sec and then signal the parent
            nanosleep(&ts, NULL);
            kill(getppid(), SIGUSR2);

            // wait until the parent responds with a signal
            while (!SIGUSR1_signal && !SIGUSR2_signal && !SIGINT_signal) {
                pause();
            } 

            if (SIGUSR1_signal) {   // guess was low
                min = guess;
            }
            if (SIGUSR2_signal) {   // guess was high
                max = guess;
            }
            if (SIGINT_signal) {   // guess was correct, so exiting the loop to begin new
                break;
            }
        }
    }
} 

void referee() {
    // setting disposition for SIGINT
    signal(SIGINT, signalHandlerP);
    
    // sleep for 5 seconds
    struct timespec ts = {5, 0};
    nanosleep(&ts, NULL);

    // send signal to children to begin the game
    kill(child1, SIGUSR1);
    kill(child2, SIGUSR2);

    // start game counter, where 10 is the no. of games/rounds
    for (int i = 0; i < 10; i++) {
        while (!SIGUSR1_signal || !SIGUSR2_signal) {
            pause();
        }
        SIGUSR1_signal = SIGUSR2_signal = 0;

        printf("\nGame %d: Player1 score: %d, Player2 score: %d\n", i+1, player1_score, player2_score);

        // generating target. adding 1 for inclusive range
        int target = rand() % 100 + 1;
        printf("Target number: %d\n", target);

        // tracker for when the game ends because of correct guess
        int game_won = 0;
        while (!game_won) {
            while (!SIGUSR1_signal || !SIGUSR2_signal) {
                pause();
            }
            SIGUSR1_signal = SIGUSR2_signal = 0;

            // open the txt files and read the guesses
            int guess1, guess2;
            int player1Fd = checkError(open("player1.txt", O_RDONLY), "issue opening");
            int player2Fd = checkError(open("player2.txt", O_RDONLY), "issue opening");
            checkError(read(player1Fd, &guess1, sizeof(int)), "issue reading");
            checkError(read(player2Fd, &guess2, sizeof(int)), "issue reading");
            close(player1Fd);
            close(player2Fd);

            printf("Player1 guesses: %d, Player2 guesses: %d\n", guess1, guess2);

            if (guess1 < target) {
                kill(child1, SIGUSR1);
            } else if (guess1 > target) {
                kill(child1, SIGUSR2);
            } else if (guess1 == target) {
                player1_score++;
                game_won = 1;
            }

            if (guess2 < target) {
                kill(child2, SIGUSR1);
            } else if (guess2 > target) {
                kill(child2, SIGUSR2);
            } else if (guess2 == target) {
                player2_score++;
                game_won = 1;
            }

            // if either player won the game, break the loop
            // and send SIGINT to both children to tell them to reset game play loop
            if (game_won) {
                printf("Game ended. Next round");
                kill(child1, SIGINT);
                kill(child2, SIGINT);
                break;
            }
        }
        
        // reset signals after each game
        SIGUSR1_signal = SIGUSR2_signal = 0;
        while (!SIGUSR1_signal || !SIGUSR2_signal) {
            pause();
        }
    }

    // print final results
    printf("\nFinal result:\nPlayer1 score is: %d\nPlayer2 score is: %d\n", player1_score, player2_score);

    kill(child1, SIGTERM);
    kill(child2, SIGTERM);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signalHandlerP;
    
    // Set dispositions for SIGUSR1, SIGUSR2, and SIGCHLD
    checkError(sigaction(SIGUSR1, &sa, NULL), "SIGUSR1 issue");
    checkError(sigaction(SIGUSR2, &sa, NULL), "SIGUSR2 issue");
    checkError(sigaction(SIGCHLD, &sa, NULL), "SIGCHLD issue");
    checkError(sigaction(SIGINT, &sa, NULL), "SIGINT issue");

    // spawn child 1
    child1 = checkError(fork(), "fork fail");
    if (child1 == 0) {
        player1();
        exit(EXIT_SUCCESS);
    }

    // spawn child 2
    child2 = checkError(fork(), "fork fail");
    if (child2 == 0) {
        player2();
        exit(EXIT_SUCCESS);
    }

    // required to randomize target number everytime
    srand(time(NULL));
    // calling parent function
    referee();

    exit(EXIT_SUCCESS);
}