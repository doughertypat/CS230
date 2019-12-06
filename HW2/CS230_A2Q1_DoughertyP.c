//CSci230 - Assignment 2 Question 1
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//16Sep19

// C program for reverse traversal of a tree without using built in recursion
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

const int false = 0;


struct node
{
	int data;
	struct node* left;
	struct node* right;
};

struct node* newNode(int data)
{
	struct node* node = (struct node*) malloc(sizeof(struct node));
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	return(node);
}

struct stack
{
	int top;
	unsigned capacity;
	struct node* *array;
};

struct stack* createStack(unsigned capacity)
{
	struct stack* stack = (struct stack*) malloc(sizeof(struct stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = (struct node**)malloc(stack->capacity * sizeof(struct node*));
	return(stack);
}

int isFull(struct stack* stack)
{
	return stack->top == stack->capacity - 1;
}

int isEmpty(struct stack* stack)
{
	return stack->top == -1;
}

void push(struct stack* stack, struct node* node)
{
	if (isFull(stack)){
		printf("Stack overflow\n");
		return;
	}
	stack->array[++stack->top] = node;

}

struct node* pop(struct stack* stack)
{
	if (isEmpty(stack)){
		printf("Stack is empty\n");
		return NULL;
	}
		
	return stack->array[stack->top--];
}

struct node* peek(struct stack* stack)
{
	if (isEmpty(stack)){
		return NULL;
	}

	return stack->array[stack->top];
}

void printRevInOrder(struct node* node)
{
	struct stack* nodeStack = createStack(20);

	struct node* tempNode = node;
	while(tempNode != NULL || isEmpty(nodeStack) == false)
	{
		while(tempNode != NULL)
		{
			push(nodeStack, tempNode);
			tempNode = tempNode->right;
		}
		tempNode = pop(nodeStack); 
		printf("%d ", tempNode->data);
		tempNode = tempNode->left;
	}

}

void printRevPostOrder(struct node* node)
{
	struct stack* nodeStack = createStack(20);
	do
	{
		while(node) //while current node is not null ->move to furthest right node
		{
			if(node->left){ //if there is a left node
				push(nodeStack, node->left); //push left node to stack
			}
			push(nodeStack, node); //push current node to stack
			node = node->right; //set current node to right child
		}
		node = pop(nodeStack); //pop last pushed node from stack
		if(node->left && peek(nodeStack) == node->left) //if left node equals top of node stack
		{
			pop(nodeStack); //discard node on top of node stack
			push(nodeStack, node); //add current node to stack
			node = node->left; //move to left node
		}
		else //otherwise print current node data
		{
			printf("%d ", node->data);
			node = NULL;
		}
	}
	while(!isEmpty(nodeStack)); //continue while node stack is not empty
}

void printRevPreOrder(struct node* node)
{
	struct stack* nodeStack = createStack(20);
	do
	{
		if(!isEmpty(nodeStack)) {
			node = pop(nodeStack);
		}
		while(node)
		{
			printf("%d ", node->data);
			if(node->left) {
				push(nodeStack, node->left);
			}
			node = node->right;	
		}
	
	} while(!isEmpty(nodeStack));
}


//Test code
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
printf("Reversed Postorder traversal of binary tree is:\n");
printRevPostOrder(root);
printf("\nReversed Inorder traversal of binary tree is:\n");
printRevInOrder(root);
printf("\nReversed PreOrder traversal of bianry tree is:\n");
printRevPreOrder(root);
printf("\n");
return 0;
}
