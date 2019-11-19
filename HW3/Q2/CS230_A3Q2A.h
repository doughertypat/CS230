//CS230 Assignment 3 Question 2
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//4 November 2019

#ifndef CS230HW3A
#define CS230HW3A

union Data{
	char op;
	int val;
};


struct node
{
        int type;
	union Data data;
        struct node* left;
        struct node* right;
        struct node* parent;
};


struct stack
{
	int top;
	unsigned capacity;
	struct node** array;
};

int isFull(struct stack* stack);

int isEmpty(struct stack* stack);

void push(struct stack* stack, struct node* node);

struct node* pop(struct stack* stack);

struct node* peek(struct stack* stack);

int opPrecidence(struct stack* stack, char op2);

int determineToken(char token);

void printInOrder(struct node* node);

void printPreorder(struct node* node);

struct node* buildTree();

#endif
