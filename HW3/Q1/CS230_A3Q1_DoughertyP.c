//CSci230 - Assignment 3 Question 1
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//25Oct19

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct node
{
	int data;
	struct node* left;
	struct node* right;
};

struct node* newNode(int data)
{
	struct node* node = (struct node*) malloc(sizeof(struct node));
	node->data=data;
	node->left = NULL;
	node->right = NULL;
	return(node);
}

struct node* nodeArray[30]={NULL};

int populateQueue(struct node* root)
{
	nodeArray[0] = root;
	nodeArray[1] = NULL;
	int i = 0;
	int top = 2;
	int childFound = 1;
	while (childFound){
		childFound = 0;
		while (nodeArray[i]){
			if (nodeArray[i]->right){
				nodeArray[top] = nodeArray[i]->right;
				top = top + 1;
				childFound = 1;
			}
			if (nodeArray[i]->left){
				nodeArray[top] = nodeArray[i]->left;
				top = top + 1;
				childFound = 1;
			}
			i = i + 1;
		}
		i = i + 1;
		top = top + 1;
	}
	return top-2;
}

int main()
{
		
struct node* root = newNode(1);
root->left = newNode(-2);
root->right = newNode(-3);
root->left->left = newNode(4);
root->left->right = newNode(5);
root->right->left = newNode(6);
root->right->right = newNode(7);
root->left->right->left = newNode(-8);
root->left->right->right = newNode(-9);
root->right->right->left = newNode(10);
root->right->right->right = newNode(11);
root->right->right->right->left = newNode(-12);
root->right->right->right->right = newNode(-13);
root->right->right->right->right->left = newNode(14);
	
int top = populateQueue(root);
int level = 0;
int sum = 0;
int seq = 1;
if (top > 0){
	sum = nodeArray[0]->data;
	printf("Level 0: ");
	printf("(seq_num=1, value=%d),", nodeArray[0]->data);
	for (int i=1;i<top;++i){
		if(!nodeArray[i]){
			printf(" sum=%d\n", sum);
			sum = 0;
			seq = 1;
			level = level + 1;
			printf("Level %d: ", level);
		}
		else{
			sum = sum + nodeArray[i]->data;
			printf("(seq_num=%d, value=%d),",seq,nodeArray[i]->data);
			seq = seq + 1;
		}
	}
}

return 0;
}
