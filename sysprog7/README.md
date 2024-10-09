# Program 7: The Hi - Lo Game implemented with Multiple Processes (Test 2)

This program simulates a guessing game between a referee and two players. The game will work as follows:
* Each player tells the referee they are ready to play
* The referee chooses a random number between 1 and 100 (inclusive)
* Tell the players to make their guess.
* Each player hands the referee their guess (this information isn’t shared between players).
* The referee then tells each player whether the guess is less then, greater than, or equal to the referee’s number.
* If no player has guessed the correct number, make a new guess.

Each player uses a different strategy to make their guess; each strategy has the following steps in common:
* Player 1 and Player 2 both maintain a lower bound and an upper bound for their guesses. The lower bound starts at 0; the upper bound starts at 100.
* The players make their guess based on these boundaries.
* Each player will then adjust their bounds based on the information provided by the referee.
* A player will set the lower bound to the current guess if that guess was lower than the referee’s value.
* A player will set the upper bound to the current guess if that guess was larger than the referee’s value.
* Both players reset the upper and lower bounds if one or both players guess correctly.

Each player’s step 2 is different. Player 1 always chooses the average of the lower and upper bounds as the next guess; Player 2 always chooses a random number between the upper and lower bounds as the next guess.

Each program will play the game 10 times.

The program plays the game using multiple concurrent processes, signals, and files.

I have included a possible design for this program.

Parent's Signal Handler
* should support SIGUSR1, SIGUSR2, SIGINT, and SIGCHLD.
* SIGUSR1 and SIGUSR2 represent the handshakes from the respective children
    * In my code I maintain a pair of globals to indicate which of these signals I have handled.
* SIGINT should send a SIGTERM to both children.
* SIGCHLD should reap the children

Children's Signal Handler
* should support SIGUSR1, SIGUSR2, SIGINT, and SIGTERM.
* SIGUSR1, SIGUSR2, and SIGINT represent the responses from the parent
    * SIGUSR1 represents a low guess
    * SIGUSR2 represents a high guess
    * SIGINT tells the child to stop guessing a start over
    * as with the parent, I use globals to indicate which of these signals I have handled
* SIGTERM signal should exit the child.

The Children (Player 1 and Player 2)
Child 1 handles Player 1’s behavior;
Child 2 handles Player 2’s behavior.

* Set the dispositions for SIGUSR1, SIGUSR2, SIGTERM, and SIGINT
* Wait until the parent has sent SIGUSR1 or SIGUSR2
* Loop forever -- game loop
    * Set min to 0, max to 101
    * Signal the parent
        * In my game, child 1 sends SIGUSR1, child 2 sends SIGUSR2
    * Loop forever -- guess loop
        * Set the flags that store handling SIGUSR1, SIGUS2, and SIGINT to 0
        * Each child should guess a value based on its strategy (see below)
        * Write the guess into a file. Each child should have its own file.
        * Sleep for 1 second
        * Signal the parent
                * In my game, child 1 sends SIGUSR1, child 2 sends SIGUSR2
        * Wait until the parent responds with a signal
        * If the parent responds with SIGUSR1 the guess was low, so set min to the guess
        * If the parent responds with SIGUSR2 the guess was high, so set max to the guess
        * If the parent responds with SIGINT the parent is moving to the next game, exit the guess loop

Player 1
* Player 1 should compute its guess by taking the average of min and max

Player 2
* Player 2 should compute its guess by generating a random number between min and max
Remember to seed the random number generator using srand; it is a good idea to the
generate a few random numbers to prevent the parent and child 2 from generating the same
random numbers every time.

The Parent (the Referee)
* Set the disposition for SIGINT
* Sleep for 5 seconds
* Tell the children to begin playing the game
    * In my game, I do this by sending SIGUSR1 to child 1 and SIGUSR2 to child 2
* Loop from 0 to 10 (this is the game counter)
    * Wait until both children have signaled they are ready to begin
        * This basically means you are waiting until the parent handles both SIGUSR1 and SIGUSR2
    * Display statistics for the game ... so display game number, total wins per player, etc
    * Generate the target for this game -- the target is a random number between 1 and 100
    * Loop forever -- referee loop
        * Wait until both child have signaled they have made a guess
            * This basically means you are waiting until the parent handles both SIGUSR1 and SIGUSR2
        * Open the files containing the guesses and read the guesses
        * Perform the following step for each player's guess
        * Compare the guess with the target and do the following
            * if guess < target, send SIGUSR1 to the correct child
            * if guess > target, send SIGUSR2 to the correct child
            * if guess == target, increase the players score by 1
            * if guess == target, store that a player has won the current game
        * If either player has won the current game, break the referee loop
    * send SIGINT to both children to tell them to reset their game play loop
* Print the final results

Main
* Set the dispositions for SIGUSR1, SIGUSR2, and SIGCHLD
* Use fork to spawn child 1 and child 2.
* Call the parent function
