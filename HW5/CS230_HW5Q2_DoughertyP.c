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
//#include <string.h>
//#include <sys/shm.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/wait.h>

//*******************************************************************
//
//*******************************************************************
void *mergeSortThread(void* agr)

