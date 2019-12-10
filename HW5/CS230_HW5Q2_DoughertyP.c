/*
CSci230 - Assignment 5 Question 2
Patrick Dougherty
patrick.r.dougherty@und.edu
9 December 2019

This program executes a multi-threaded merge sort. 
*/

#include <stdio.h>
#include <stdlib.h>
//#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
//#include <string.h>
//#include <sys/shm.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/wait.h>
#include "CS230_HW5Q2A_DoughertyP.h"

typedef struct{
  int *A;
  int *B;
}mergeList;

//*******************************************************************
// mergeSortThread - Function for merge sort thread recurrsion
//*******************************************************************
void *mergeSortThread(void* arg){
  //FIXME: finish this

  int *list = (int*)arg;
  for(int i=0;i<4;i++){
    printf("%d, ", list[i]);
  }
  printf("\n");
  pthread_exit(NULL);
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
  if(!isdigit(*argv[1])){
    printf("Improper usage: argument must be an integer (such as 7)");
    exit(1); 
  } 
//Capture command-line argument
  int argNum = atoi(argv[1]);
//Generate random list of integers
  int *unsortedList = randomListGen(argNum);
  for(int i=0; i<argNum; i++){
    printf("%d, ", unsortedList[i]);
  }
  printf("\n");
//Sort that mess
  pthread_t initial_thread;
  pthread_create(&initial_thread, NULL, mergeSortThread, unsortedList);

  pthread_join(initial_thread, NULL);




}
