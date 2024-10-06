# Program 5 (Timed Quiz)

Write a program using systems C that implements a timed quiz. 
Your program needs to meet the behavioral and structural requirements
detailed below.

Your timed quiz needs do the following:
1. Inform the user they are about to begin a timed quiz and ask if they wish to proceed.
   It is probably best to explain how long the user has to answer each question (15 seconds)
   and how they can exit the program using Ctl-C. 
2. Read a question-and-answer pair from the files "quest.txt" and "ans.txt" respectively. 
   You will repeat this for as long as there are question/answer pairs available.
2.a. Display the current question.
2.b. Prompt the user for an answer
2.b.i. Wait for the user to enter a response or for timer to time out (15 seconds)
2.c. If the user entered a response, compare it against the answer and record whether it
     was right or wrong; if the timer timed out, report that "Time has elapsed."
     In either case, move to the next question.
3. Display the final results of the quiz.

The program you write must incorporate the following:
1. All file I/O should be accomplished using the "read", "write", "open", and "close" system calls. 
   talked about in class; failure to do so will result in the loss of points.
2. When altering the disposition of a signal, use "struct sigaction" and the "sigaction system call.
3. The response timer should be implemented using the ITIMER_REAL interval timer; remember that the timer has
   a value of 15 seconds.
4. Pressing Ctl-C should print the "Exit (y/N)?" message and handle it accordingly.
