/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "BST.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	struct BST* store = newBST();
	char word[20];

	printf("Please enter a series of words to store in a BST\n");
	printf("Type 'STOP' to end the series and output the BST\n");
	printf("In postorder\n");


	while (1)
	{
		printf("Word = ");
		scanf("%s", word);
		if (strcmp(word, "STOP") == 0) break;
		insert(store, word);
	}

	printf("The BST ouput in postorder is \n");
	postOrder(store->root);
	printf("\n\n");

    //free the tree
    dealloc(store->root);
	free(store);


	return 1;
}
