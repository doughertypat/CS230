/*
CSci230 - Assignment 5 Question 2
Patrick Dougherty
patrick.r.dougherty@und.edu
9 December 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//*******************************************************************
// randomListGen - Returns an array or randomly generated integers
//*******************************************************************
int* randomListGen(int length){
  time_t t;
  int *randList;
  randList = (int*)malloc(length*sizeof(int));
  srand(time(&t));
  for(int i = 0; i<length; i++){
      randList[i] = rand()%1001;
  }
  return randList;
}
//*******************************************************************
// merge - merges to array keeping values in increasing order.
//	   Input arrays must already in increasing order.
//*******************************************************************
int* merge(int *A, int *B, int size){
  int *C;
  int n1 = size/2;
  int n2 = size-n1;
  C = (int*)malloc(size*sizeof(int));
  int i=0, j=0;
  while(i<n1 && j<n2){
    if (A[i]<=B[j]){
      C[i+j] = A[i];
      i += 1;
    }
    else{
      C[i+j] = B[j];
      j += 1;
    }
  }
  while(i<n1){
    C[i+j] = A[i];
    i += 1;
  }
  while(j<n2){
    C[i+j] = B[j];
    j += 1;
  }
  return C;
}


/*
//Driver code to test functions.  REMOVE BEFORE USING THIS FILE!
int main(void){
  int length = 4;
  int *unsortedList = (int*)malloc(length*sizeof(int));
  int *sortedList = (int*)malloc(length*sizeof(int));
  unsortedList = randomListGen(length);
  for(int i=0; i<length; i++){
    printf("%d\n", unsortedList[i]);
  }
  int unsortedList1[2] = {unsortedList[0], unsortedList[1]};
  int unsortedList2[2] = {unsortedList[2], unsortedList[3]};
  sortedList = merge(unsortedList1, unsortedList2);
  for(int i=0; i<length; i++){
    printf("%d, ", sortedList[i]);
  }
  printf("\n");
  return 0;
}
*/
