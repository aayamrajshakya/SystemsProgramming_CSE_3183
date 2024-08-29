#include "BST.h"
#include <stdbool.h>  //ref: https://www.w3schools.com/c/c_booleans.php
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//defining a new node and its tree-properties
struct Node* newNode(const char *value) {
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->value = strdup(value);
	temp->right = NULL;
	temp->left = NULL;
	return temp;
}

//create new BST
struct BST* newBST() {
	struct BST *tree = (struct BST *)malloc(sizeof(struct BST));
	tree->root = NULL;
	return tree;
}


int insert(struct BST *tree, const char *value) {
    struct Node **node = &(tree->root);

     //run this while tree isn't empty and find a spot for the to-be-inserted node
    while (*node != NULL) {
        if (strcmp(value, (*node)->value) < 0) {
            node = &((*node)->left);   //smaller value goes to the left
        } else if (strcmp(value, (*node)->value) > 0) {
            node = &((*node)->right);  //larger value always to the right side
        } else {
            return false;  //duplication or repetition of value in tree isn't allowed
        }
    }

    //inserting the new node when we find a spot
    if (*node == NULL) {
        *node = newNode(value);
        return true;
    }
    return false;
}

void postOrder(struct Node *node) {
	if (node == NULL) return;
	postOrder(node->left);
	postOrder(node->right);
	printf("%s ", node->value);
}

//not sure if this even works in my program lol
void dealloc(struct Node *node) {
	if (node == NULL) return;
	dealloc(node->left);
	dealloc(node->right);
	free(node);
}


/* I commented out unused functions to save me some time

bool BST::search(Node *node, const std::string &value)
{
  if (node == nullptr)
    {
      return false;
    }
  if (value < node->value) { return search(node->left,value);}
  if (value > node->value) { return search(node->right,value);}
  return true;
}

bool BST::remove(Node *&node, const std::string &value)
{
  return true;
}

void BST::copy(Node *node, BST *bstCp)
{
  if (node == nullptr) return;
  bstCp->insert(node->value);
  copy(node->left,bstCp);
  copy(node->right,bstCp);
}

*/