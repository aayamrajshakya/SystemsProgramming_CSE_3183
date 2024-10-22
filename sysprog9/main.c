#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


// global variable to store game, guess, and comparison state
static int cmp[2], rdy[4], guess[2];

// initializing the players score
int player1_score = 0, player2_score = 0;


// creating m3 mutexes and corresponding condition variables
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mtx3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

// Mr. Knight's flagship error checker
int checkError(int val, const char* msg) {
	if (val == -1) {
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}

// Mr. Knight's thread error checker
int threadError(int val, const char *msg)
{
	if (val == 0) return 0;
	errno = val;
	perror(msg);
	exit(EXIT_FAILURE);
}

// player 1 thread
void *player1(void *args) {

	// 1st forever loop
	while(1) {
		int min = 0, max = 100;
		threadError(pthread_mutex_lock(&mtx3), "mutex_lock");

		// using while loop to wait on condvar 3
		while(rdy[2] == 0) {
			threadError(pthread_cond_wait(&cond3, &mtx3), "cond_wait");
		}

		// unlocking mutex 3 and setting rdy flag to 0
		threadError(pthread_mutex_unlock(&mtx3), "mutex_unlock");
		rdy[2] = 0;

		// 2nd forever loop
		while(1) {
			// guessing strategy for player 1
			guess[0] = (min + max) / 2;
			threadError(pthread_mutex_lock(&mtx1), "mutex_lock");

			while(rdy[0] == 0) {
				threadError(pthread_cond_wait(&cond1,&mtx1), "cond_wait");
			}
			threadError(pthread_mutex_unlock(&mtx1), "mutex_unlock");

			rdy[0] = 0;

			if(cmp[0] < 0) {
				printf("Player 1 guessed smaller: %d\n", guess[0]);				
				min = guess[0];
			}
			else if (cmp[0] > 0) {
				printf("Player 1 guessed larger: %d\n", guess[0]);				
				max = guess[0];
			}
			else {
				break;
			}
		}
	}
}

// player 2 thread
// the explanation comment are same as for player 1
void *player2(void *args) {

	// seeding the number generator for player 2
	srand(time(NULL));

	// 1st forever loop
	while(1) {
		int min = 0, max = 100;
		threadError(pthread_mutex_lock(&mtx3), "mutex_lock");
		while(rdy[3] == 0) {
			threadError(pthread_cond_wait(&cond3, &mtx3), "cond_wait");
		}
		threadError(pthread_mutex_unlock(&mtx3), "mutex_unlock");
		rdy[3] = 0;

		// 2nd forever loop
		while(1) {

			// strategy of guessing random no. between min and max
			guess[1] = (rand() % (max - min + 1)) + min;
			threadError(pthread_mutex_lock(&mtx2), "mutex_lock");

			while(rdy[1] == 0) {
				threadError(pthread_cond_wait(&cond2,&mtx2), "cond_wait");
			}
			threadError(pthread_mutex_unlock(&mtx2), "mutex_unlock");

			rdy[1] = 0;

			if(cmp[1] < 0) {
				printf("Player 2 guessed smaller: %d\n", guess[1]);
				min = guess[1];
			}
			else if (cmp[1] > 0) {
				printf("Player 2 guessed larger: %d\n", guess[1]);
				max = guess[1];
			}
			else {
				break;
			}
		}
	}
}

void *referee(void *args){
	struct timespec ts = {1, 0};
	srand(time(NULL));

	// loop for 10 times, which is the number of games to be played
	for (int i = 0; i < 10; i++){
		threadError(pthread_mutex_lock(&mtx3), "mutex_lock");

		// retrieving referee's target
		int target = rand() % 100 + 1;
		rdy[2] = rdy[3] = 1;

		// broadcasting condvar3 and unlocking mutex 3
		threadError(pthread_cond_broadcast(&cond3),"cond_signal");
		threadError(pthread_mutex_unlock(&mtx3), "mutex_unlock");

		// displaying target and game statistics
		printf("\nThe target is: %d\n", target);
		printf("\nGame %d: Player 1 score: %d, Player 2 score: %d\n", i+1, player1_score, player2_score);

		// forever loop
		while(1){
			nanosleep(&ts, NULL);
			threadError(pthread_mutex_lock(&mtx1), "mutex_lock");
			threadError(pthread_mutex_lock(&mtx2), "mutex_lock");

			rdy[0] = rdy[1] = 0;
			
			cmp[0] = guess[0] - target;
			cmp[1] = guess[1] - target;

			rdy[0] = rdy[1] = 1;

			threadError(pthread_cond_broadcast(&cond1),"cond_signal");
			threadError(pthread_cond_broadcast(&cond2),"cond_signal");
			threadError(pthread_mutex_unlock(&mtx1), "mutex_unlock");
			threadError(pthread_mutex_unlock(&mtx2), "mutex_unlock");

			// if player 1 made a correct guess, run this
			if(cmp[0] == 0){

				// reinitialize player2's referee response for next round
				cmp[1] = 0;
				printf("Player 1 guessed correct: %d\n", guess[0]);
				player1_score++;
				printf("\nPlayer 1 won the round\n");
				// guess[0] = 0;
				break;
			}

			// if player 2 made a correct guess, run this
			if(cmp[1] == 0){

				// reinitialize player1's referee response for next round
				cmp[0] = 0;
				printf("Player 2 guessed correct: %d\n", guess[1]);
				player2_score++;
				printf("\nPlayer 2 won the round\n");
				// guess[1] = 0;
				break;
			}
		}
	}

	// display final game stats
    printf("\nFinal result:\nPlayer 1 score is: %d\nPlayer 2 score is: %d\n", player1_score, player2_score);
}

int main(){

	// declaring threads for player1, player2, and referee
	pthread_t p1_thread, p2_thread, ref_thread;
	srand(time(NULL));
	rdy[0] = rdy[1] = rdy[2] = rdy[3] = 0;

	// spawing threads for player1, player2, and referee
	threadError(pthread_create(&p1_thread, NULL, player1, NULL), "pthread_create");
	threadError(pthread_create(&p2_thread, NULL, player2, NULL), "pthread_create");
	threadError(pthread_create(&ref_thread, NULL, referee, NULL), "pthread_create");

	// joining the referee thread
	threadError(pthread_join(ref_thread, NULL), "pthread_join");
	printf("Exiting\n");
	return 0;
}