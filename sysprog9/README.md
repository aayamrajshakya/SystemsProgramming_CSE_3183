# Program 9: The HI-Lo Game implemented using Threads (Test 2)

Each program is simulating a guessing game between a referee and two players. The game will work as follows:
1) Each player tells the referee they are ready to play
2) The referee chooses a random number between 1 and 100 (inclusive)
3) Tell the players to make their guess.
4) Each player hands the referee their guess (this information isn’t shared between players).
5) The referee then tells each player whether the guess is less then, greater than, or equal to the referee’s number. If no player has guessed the correct number, repeat steps 3 – 5.

Each player uses a different strategy to make their guess; each strategy has the following steps in common:
1) Player 1 and Player 2 both maintain a lower bound and an upper bound for their guesses. The lower bound starts at 0; the upper bound starts at 100.
2) The players make their guess based on these boundaries.
3) Each player will then adjust their bounds based on the information provided by the referee.
   1) A player will set the lower bound to the current guess if that guess was lower than the referee’s value.
   2) A player will set the upper bound to the current guess if that guess was larger than the referee’s value.
   3) Both players reset the upper and lower bounds if one or both players guess correctly.

Each player’s step 2 is different. Player 1 always chooses the average of the lower and upper bounds as the next guess; Player 2 always chooses a random number between the upper and lower bounds as the next guess.
Each program will play the game 10 times.
What follows are the details I feel are necessary to help you complete each option.
You will need some global variables to save the shared data and manage the state of the game. I used the following global variables:
* guess[] stores each thread’s guess
* cmp[] stores the referee’s response to each thread
   * < 0 means guess < target
   * 0 means guess = target
   * > 0 means guess > target
* rdy[] stores the various flags need for the condition variables
    * 0 means the condition variable hasn't been signaled
    * 1 means the condition variable has been signaled
* mtx[] stores the mutexes
* cnd[] stores the condition variables

Thread 1 handles Player 1’s behavior; Thread 2 handles Player 2’s behavior. Both threads share a common structure.
1) Loop forever
   1) Set min to 0 and max to 100
   2) Lock mutex 3
   3) Using a while loop, wait on condition variable 3.
In my code base this was implemented as a while(!rdy) cond_wait() loop. The following loop is for thread 1
while (rdy[2] == 0) pthread_cond_wait(&cnd[2], &mtx[2]);
(note: both players are waiting on the cnd[2], mtx[2] pairing)
    4) Unlock mutex 3 and set the rdy flag you were looping over to 0
    5) Loop forever
         1) Calculate the child’s guess using the proper method (See below)
         2) Store that guess in the proper global variable.
         3) Lock the mutex associated with the thread’s guess (See below)
         4) Using a while loop, wait on the associated condition variable (See below)
         5) Unlock the proper mutex (See below)
         6) Once this condition variable has been signaled, the value of the referee’s response should be stored in the cmp global. Using the proper element of the cmp global do the following:
              1) If cmp < 0, set min to the current guess
              2) If cmp > 0, set max to the current guess
              3) If cmp is 0, break.
           
I suggest using the following values for Thread 1:
* use mtx[0] as the thread's mutex for its guess
* use cnd[0] and rdy[0] as your condition variable and associated state
* use guess[0] and cmp[0] to store thread 1's guess and the referee's response

Thread 1 computes its guess by taking the average of min and max
I suggest using the following values for Thread 2:
* use mtx[1] as the thread's mutex for its guess
* use cnd[1] and rdy[1] as your condition variable and associated state
* use guess[1] and cmp[1] to store thread 2's guess and the referee's response

Thread 2 computes its guess by generating a random number between min and max.
Remember to seed the random number generator when Thread 2 starts; you might also need to generate a few initial random numbers so that Thread 2 isn’t always generating the same random numbers as the referee.
The Referee thread should follow the outline below:
1) Loop from 0 to 10
     1) Lock mutex 3
        Generate the target for this game. Recall this is a random number between 1 and 100 (inclusive).
     2) Set the two rdy flags associated with condition variable 3 to 1. I suggest using rdy[2] and rdy[3].
     3) Broadcast condition variable 3 and unlock mutex 3
     4) Print the game statistics (which game you are in, the current scores, etc)
     5) Loop forever
          1) Sleep for 1 second
          2) Lock mutexes 1 and 2
          3) Set the rdy flags for each mutex-condition variable pairing to 0.
              I suggest using rdy[0] for mutex1-condition variable 1 and rdy[1] for mutex2-condition variable 2.
          4) Set each cmp[0] to guess[0] - target and cmp[1] to guess[1] - target
          5) Set both rdy[0] and rdy[1] to 1.
          6) Broadcast condition variables 1 and 2 and unlock mutexes 1 and 2
          7) If either thread made a correct guess, break. A thread made a correct guess if cmp is 0. Make certain to set both cmp's to 0 in this case.
2) Print the final game statistics

The main function should follow the outline below:
1) Seed the random number generator
2) Set the rdy flags to 0
3) Spawn threads 1, 2, and the referee
4) Join the referee thread
