/*
CSci230 - Assignment 5 Question 2
Patrick Dougherty
patrick.r.dougherty@und.edu
9 December 2019

This program executes a multi-threaded merge sort. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include "CS230_HW5Q2A_DoughertyP.h"

//Global variable declarations
typedef struct{
  int *A;
  int *B;
  int size;
}mergeList;

//*******************************************************************
// mergeSortThread - Function for merge sort thread recurrsion
//*******************************************************************
void *mergeSortThread(void* arg){
//Convert passed args
  mergeList list = *((mergeList*)arg);
//Check size and return
  printf("Input: %d", list.A[0]);
  for(int i=1; i<list.size; i++){
    printf(", %d", list.A[i]);
  }
  printf("\n");
  if(list.size == 1){
    pthread_exit(list.A); 
  }
//Partition array
  int mid = list.size/2;
  int A[mid];
  int B[list.size-mid];
  for(int i = 0; i<mid; i++){
    A[i] = list.A[i];
  }
  for(int i = mid; i<list.size; i++){
    B[i-mid] = list.A[i];
  }
  mergeList list1, list2;
  list1.size = mid;
  list1.A = A;
  list2.size = list.size-mid;
  list2.A = B;
//Pass partitioned arrays to new theads to recurrsively sort 
  pthread_t thread1, thread2; 
  pthread_create(&thread1, NULL, mergeSortThread, &list1);
  pthread_create(&thread2, NULL, mergeSortThread, &list2);
//Join threads and capture reutrned values
  void *rtn1 = NULL;
  void *rtn2 = NULL;
  if(pthread_join(thread1, &rtn1)!=0){
    perror("join 1");
  }
  if(pthread_join(thread2, &rtn2)!=0){
    perror("join 2");
  }
  int *y = (int*)rtn1;
  int *z = (int*)rtn2;
  printf("Return A: %d", y[0]);
  for(int i=1; i<mid; i++){
    printf(", %d", y[i]);
  }
  printf("\n");
  printf("Return B: %d", z[0]);
  for(int i=1; i<list.size-mid; i++){
    printf(", %d", z[i]);
  }
  printf("\n");
//Merge and return sorted array
  int* returnList = merge((int*)rtn1, (int*)rtn2, list.size); 
  pthread_exit(returnList);
}

//*******************************************************************
// main - Driver Function
//*******************************************************************
int main(int argc, char **argv){
//First check the user gave us something useable
  if(argc < 2){
    printf("Improper usage: CS230_HW5Q2_DoughertyP <# of integers to create>\n");
    exit(1);
  }
  if(!isdigit(*argv[1]) || atoi(argv[1]) == 0){
    printf("Improper usage: argument must be an integer greater than 0 (such as 7)\n");
    exit(1); 
  } 
//Capture command-line argument
  int argNum = atoi(argv[1]);
//Generate random list of integers
  int *unsortedList = randomListGen(argNum);
  printf("Unsorted list: %d", unsortedList[0]);
  for(int i=1; i<argNum; i++){
    printf(", %d", unsortedList[i]);
  }
  printf("\n");
  mergeList list;
  list.size=argNum;
  list.A = unsortedList;
//Sort that mess
  pthread_t initial_thread;
  pthread_create(&initial_thread, NULL, mergeSortThread, &list);
  void* sList = NULL;
  pthread_join(initial_thread, &sList);
//Print sorted list
  int* sortedList = (int*)sList;
  printf("Sorted list: %d", sortedList[0]);
  for(int i = 1; i<argNum; i++){
    printf(", %d", sortedList[i]);
  }
  printf("\n");
  return 0;
}
