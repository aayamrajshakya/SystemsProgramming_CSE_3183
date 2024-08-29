#ifndef BST_H
#define BST_H

struct Node{
    char *value;
	struct Node *left;
	struct Node *right;
};

struct BST{
    struct Node *root;
};

struct BST* newBST();
int insert(struct BST *tree, const char *value);
void postOrder(struct Node *node);
void dealloc(struct Node *node);

#endif
