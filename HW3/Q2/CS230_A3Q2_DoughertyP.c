//CSci230 - Assignment 3 Question 2
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//04 November 2019

#include<stdio.h>
#include<stdlib.h>
#include"CS230_A3Q2A.h"
#include"CS230_A3Q2B.h"

int main()
{
	struct node* root = buildTree();
	printf("The expression you entered converted to prefix notation is:\n");
	printPreorder(root);
	printf("\n");
	int res = evalMathTree(root);
	printf("The result is:%d\n", res);
	return 0;
}

