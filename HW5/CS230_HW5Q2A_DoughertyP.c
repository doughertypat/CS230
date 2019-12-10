/*
CSci230 - Assignment 5 Question 2
Patrick Dougherty
patrick.r.dougherty@und.edu
9 December 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

int main(void){
  int length = 7;
  int *unsortedList = (int*)malloc(length*sizeof(int));
  unsortedList = randomListGen(length);
  for(int i = 0; i < length; i++){
    printf("%d, ", unsortedList[i]);
  }
  printf("\n");
  return 0;
}
