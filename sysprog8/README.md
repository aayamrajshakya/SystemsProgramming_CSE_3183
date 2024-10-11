# Program 8: Using exit, exec, and wait

For this exercise, we will be using the system and exit system calls.
We will accomplish this by writing two independent programs, instead of a normal parent-child pairing.
The first program should be called myRand and does the following:
1. Generate a new file named dataX.dat, where X is a random number between 0 and 255.
2. Generate 60 values between 0 and 100 and store them in the file.
3. Exit with an exit status of X.

The second program should do the following:
1. Use fork to create a child process and use one of the exec functions to run the myRand program. You can simply use wait or waitpid for this one; you don't need to write a signal handler for SIGCHLD unless you choose to.
    * Remember that you need to use WEXITSTATUS to convert the status returned by wait to the actual value.
2. Open the file dataX.dat, where X is the number returned from system.
3. Read the 60 values from the file, display them, compute their average, and output the average.
4. Unlink the data file.
