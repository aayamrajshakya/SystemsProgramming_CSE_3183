# Project 4

For this assignment, you need to convert the provided program from C++ to C. You should pay particular 
attention to three primary areas of difference:
1. C++ has classes, C doesn't; this will necessitate converting a C++ class into an eqivalent C struct
2. C++ has a string type, C doesn't; C uses a mechanism known as C-strings
3. C++ use new, delete, and delete[] for memory management, C use malloc, calloc, and free
The resulting C program will be expected to use structs, C-strings, and malloc + free.


The provided program does the following:

It reads an unspecified number of strings storing them in a Binary Search Tree (BST); the program then
outputs the contents of the BST using a post-order traversal. Your program should follow the same structure
that the C++ program utilizes.

That structure is similar to the following:
1. Create and initialize all necessary data structures and variables
2. Inform the user that the program will read strings until "STOP" is entered
3. Loop
   a. Prompt the user for a string
   b. If the string is "STOP" break the loop, otherwise insert the string into the BST
4. Inform the user that the data they will now be displayed in post-order
5. Display all the contents of the BST using a post-order traversal.
6. Free the space used by the BST to store the strings 
