//CS230 Assignment 3 Question 2
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//28Oct19

#include<stdio.h>
#include<stdlib.h>
#include "CS230_A3Q2A.h"

//struct node;
//struct stack;

struct node* newNode(int data, int type)
{
	struct node* node = (struct node*) malloc(sizeof(struct node));
	node->data.val=data;
	node->type=type;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	return(node);
}

struct stack* createStack(unsigned capacity)
{
	struct stack* stack = (struct stack*)malloc(sizeof(struct stack));
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
		printf("Stack is empty\n");
		return NULL;
	}
	return stack->array[stack->top];
}

int opPrecidence (struct stack* stack, char op2)
{
	struct node* op1 = peek(stack);
	if ((op1->data.op=='+'||op1->data.op=='-')&&(op2=='*'||op2=='/')) {
		return 1;
	}
	else if ((op1->data.op=='*'||op1->data.op=='/')&&(op2=='+'||op2=='-')) {
		return 0;
	}
	else if (op1->data.op=='(') {
		return 1;
	}
	else {
		return 0;
	}
}

int determineToken (char token)
{
	//char token = node->data.op;
        if (token=='+'||token=='-'||token=='*'||token=='/') {
                //node->type=0;
		return 0;
        }
        else if (token=='(') {
                //node->type=1;
		return 1;
        }
        else if (token==')') {
                //node->type=2;
		return 2;
        }
        else if (token>=48 && token<=57){
                //node->type=3;
		return 3;
        }
	else {
		printf("Unknown operator detected. Exiting.\n");
		exit(0);
	}
}

void printInOrder(struct node* node)
{
if (node == NULL)
	return;
printInOrder(node->left);
if (node->type == 3) {
	printf("%d ", node->data.val); }
else {
	printf("%c ", node->data.op); }
printInOrder(node->right);
}

void printPreorder(struct node* node)
{
if (node == NULL)
	return;
if (node->type == 3){
	printf(" %d", node->data.val); }
else {
	printf("%c", node->data.op); }
printPreorder(node->left);
printPreorder(node->right);
}

struct node* buildTree() {

struct stack* opStack=createStack(100);
struct stack* valStack=createStack(100);
char exp[100];
printf("This expression evaluator only supports integers, +, -, *, /, ( and ).\n");
printf("Please enter an expression to evaluate:\n");
fgets (exp, 100, stdin);
size_t i=0;
int opPrec;
while(exp[i] != '\n' && exp[i] !='\0')
{
  if(exp[i] == ' '){
    ++i;
  }
  else{
    switch(determineToken(exp[i])){
      case 0:
	if (opStack->top == -1 && valStack->top == -1){
	  if (exp[i] == '-'){
	    push(valStack, newNode(-1,3));
	    push(opStack, newNode('*',0));
	    ++i;
	    break;
	  }
	  else if(exp[i] == '+'){
	    push(valStack, newNode(0,3));
	    push(opStack, newNode('+',0));
	    ++i;
	    break;
	  }
	}
	int j = i-1;
	  while (exp[j] == ' ') {
	    --j;
	  }
	if (determineToken(exp[j])==0||determineToken(exp[j])==1) {
	  if (exp[i] == '-')  {
	    push(valStack, newNode(-1,3));
	    push(opStack, newNode('*',0));
	    ++i;
	    break;
	  }
	  else if (exp[i] == '+') {
	    push(valStack, newNode(0,3));
	    push(opStack, newNode('+',0));
	    ++i;
	    break;
	  }
	  else if (exp[i] == '*' || exp[i] =='/') {
	    printf("Improper operator placement detected at %c, %c. Exiting.\n",
			exp[j], exp[i]);
	    exit(0);
	  }
	}
	if (opStack->top == -1 || opPrecidence(opStack,exp[i])) {
	  push(opStack, newNode(exp[i],0));
	  ++i;
	  break;
	}
	else {  
	  opPrec = opPrecidence(opStack,exp[i]);
	  while (opStack->top != -1 && !(opPrec)) {
	    peek(opStack)->right = pop(valStack);
	    peek(opStack)->left = pop(valStack);
	    push(valStack, pop(opStack));
	    if (opStack->top != -1){
	      opPrec = opPrecidence(opStack,exp[i]);
	    } 
	  }
	  push(opStack,newNode(exp[i],0));
	  ++i;
	break;
	}
    case 1:
      push(opStack,newNode(exp[i],1));
      ++i;
      break;
    case 2:
      while (peek(opStack)->type != 1) {
	if (isEmpty(opStack)){
	  printf ("Error - Unbalanced parenthesis found.\n");
	  exit(0);
	}
	peek(opStack)->right = pop(valStack);
        peek(opStack)->left = pop(valStack);
        push(valStack, pop(opStack));
      }
      pop(opStack);
      ++i;
      break;
    case 3:
      if (i > 0 && exp[i-1]!=' ' && determineToken(exp[i-1]) == 3) {
	peek(valStack)->data.val *= 10;
	if (peek(valStack)->data.val < 0) {
	  peek(valStack)->data.val -= (exp[i] - 48);
          ++i;
	  break;
	}
	else {
	  peek(valStack)->data.val += (exp[i] - 48);
	  ++i;
	  break;
	}
      }
      else {
	push(valStack, newNode((exp[i]-48),3));
	++i;
	break;
      }

    }
  }
}
while (opStack->top != -1)
{
	peek(opStack)->right = pop(valStack);
	peek(opStack)->left = pop(valStack);
	push(valStack, pop(opStack));
}
//printInOrder(peek(valStack));
//printf("\n");
//printPreorder(peek(valStack));
//printf("\n");
	
return peek(valStack);
}
