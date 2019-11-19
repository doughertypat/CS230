//CS230 Assignment 3 Question 2
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//4 November 2019

#include<stdio.h>
#include<stdlib.h>
#include"CS230_A3Q2A.h"
#include"CS230_A3Q2B.h"

int evalMathTree(struct node* node)
{
  if(!node) {
    return 0;
  }
  if(!node->left && !node->right) {
    return node->data.val;
  }
  int leftTree = evalMathTree(node->left);
  int rightTree = evalMathTree(node->right);
  if(node->data.op == '+') {
    return leftTree + rightTree;
  }
  if(node->data.op == '-') {
    return leftTree - rightTree;
  }
  if(node->data.op == '*') {
    return leftTree * rightTree;
  }
  if(node->data.op == '/') {
    if(rightTree == 0) {
      printf("Divide by zero detected. Exiting.\n");
      exit(0);
    }
    return leftTree / rightTree;
  }
  printf("Unknown operator encountered.\n");
  exit(0);
}
