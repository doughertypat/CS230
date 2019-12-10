/*
CS230 - Assignment 5 Question 1
Patrick Dougherty
patrick.r.dougherty@und.edu
4 December 2019

This program shuffles the contents of five files while maintaining the items index
values. For example the item at index zero in file one may end up at index zero in file three. The program has a producer and consumer child process who will create five threads each.  Each producer thread will read from a single file and each consumer will write to a new file.
*/

//Header file declarations
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

//Global constant declarations
#define THREAD_COUNT 5 //number of producer and consumer threads
#define BUFFER_SIZE 8 //size of circular queue in bytes
#define SEM_MUTEX "/sem_mutex"
#define SEM_FULL "/sem_full"
#define SEM_EMPTY "/sem_empty"

//Global variable declarations
sem_t *mutex;
sem_t *semFull;
sem_t *semEmpty;
pthread_barrier_t prod_barrier, con_barrier;
const char *fileName = "HW5shm";
char *shmid;
int shm_fd;

//*******************************************************************
//prodThreadFun - Producer Thread Function
//*******************************************************************
void *prodThreadFun(void* arg) {
  printf("Producer Thread %d Started\n", (int)pthread_self()); //REMOVE BEFORE FLIGHT
//Use semaphores to ensure each thread opens a unique file

  if(sem_wait(mutex)!=0){
    perror("sem_wait -p");
  }
  char fileName[9];
  if((sprintf(fileName, "Prod%d.txt", *(int*)arg)) == -1) {
    perror("sprintf");
    pthread_exit(NULL);
  }
  FILE *fd = fopen(fileName, "r");
  if(fd == NULL){
    perror("fopen -p");
    pthread_exit(NULL);
  }
  printf("Tread %d grabbed file %s\n", (int)pthread_self(), fileName); //REMOVE BEFORE FLIGHT
  *(int*)arg += 1;
  if(sem_post(mutex)!=0){
    perror("sem_post -p");
  }
//We are goign to use arg to track write location later - set to zero after last file opened
  if(*(int*)arg == THREAD_COUNT + 1){
    *(int*)arg = 0;
  }
  pthread_barrier_wait(&prod_barrier);
  
  char readChar;
  while(readChar!=EOF) {
    readChar = fgetc(fd);
    sem_wait(semFull);
    sem_wait(mutex);
    shmid[*(int*)arg] = readChar;
    *(int*)arg = (*(int*)arg + 1) % BUFFER_SIZE;
    sem_post(mutex);
    sem_post(semEmpty);
    pthread_barrier_wait(&prod_barrier);
  }
 
  pthread_exit(NULL);

}
//*******************************************************************
//conThreadFun - Consumer Thread Function
//*******************************************************************
void *conThreadFun(void* arg) {
  printf("Consumer Thread %d Started\n", (int)pthread_self()); //REMOVE BEFORE FLIGHT
//Use semaphores to ensure each thread opens a unique file
  if(sem_wait(mutex)!=0){
    perror("sem_wait -c");
  }
  char fileName[9];
  if((sprintf(fileName, "Cons%d.txt", *(int*)arg)) == -1) {
    perror("sprintf");
    pthread_exit(NULL);
  }
  FILE *fd = fopen(fileName, "w");
  if(fd == NULL){
    perror("fopen -p");
    pthread_exit(NULL);
  }
  printf("Thread %d grabbed file %s\n", (int)pthread_self(), fileName); //REMOVE BEFORE FLIGHT
  *(int*)arg += 1;
  if(sem_post(mutex)!=0){
    perror("sem_post -p");
  }
//We are goign to use arg to track write location later - set to zero after last file opened
  if(*(int*)arg == THREAD_COUNT + 1){
    *(int*)arg = 0;
  }
  pthread_barrier_wait(&con_barrier);

  char writeChar;
  while(1) {
    sem_wait(semEmpty);
    sem_wait(mutex);
    writeChar = shmid[*(int*)arg];
    *(int*)arg = (*(int*)arg + 1) % BUFFER_SIZE;
    sem_post(mutex);
    sem_post(semFull);
    pthread_barrier_wait(&con_barrier);
    if(writeChar == EOF) {
      break;
    }
    fprintf(fd, "%c", writeChar);
  }

  pthread_exit(NULL);
}
//*******************************************************************
//Producer
//*******************************************************************
int Producer() {
//Declare variables
  int count = 1;//keep count of how many files have been opened
  printf("The producer has started\n");//REMOVE BEFORE FLIGHT
//Declare and instantiate pthread barrier  
  if((pthread_barrier_init(&prod_barrier,NULL,THREAD_COUNT)) != 0) {
    perror("pthread_barrier -p");
    exit(1);
  }
//Declare and start pthreads
  pthread_t *prod_thread;
  prod_thread = malloc(THREAD_COUNT * sizeof(pthread_t));
  for(int i = 0; i < THREAD_COUNT; i++) {
    if((pthread_create(&prod_thread[i], NULL, prodThreadFun, &count)) != 0){
      perror("pthread_create -p");
      for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_cancel(prod_thread[i]);
      }
      exit(1);
    }
  }
//***troubleshooting - delete in final version
  printf("Producer threads created\n");
//Join pthreads
  for(int i = 0; i < THREAD_COUNT; i++) {
    if((pthread_join(prod_thread[i], NULL)) != 0) {
      perror("pthread_join -p");
    }
  }
  printf("Producer threads joined\n");
//Destroy pthread barrier
  if((pthread_barrier_destroy(&prod_barrier)) != 0) {
    perror("pthread_barrier_destroy -p");
  }
  return 0;
}
//*******************************************************************
//Consumer
//*******************************************************************
int Consumer() {
  int count = 1;
  printf("The consumer has started\n");
//Declare and instantiate pthread barrier  
  if((pthread_barrier_init(&con_barrier, NULL,THREAD_COUNT)) != 0) {
    perror("pthread_barrier -c");
    exit(1);
  }
//Declare and start pthreads
  pthread_t con_thread[THREAD_COUNT];
  for(int i = 0; i < THREAD_COUNT; i++) {
    if((pthread_create(&con_thread[i], NULL, conThreadFun, &count)) != 0){
      perror("pthread_create -c");
      for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_cancel(con_thread[i]);
      }
      exit(1);
    }
  }
  printf("Consumer threads created\n");
//Join pthreads
  for(int i = 0; i < THREAD_COUNT; i++) {
    if((pthread_join(con_thread[i], NULL)) != 0) {
      perror("pthread_join -c");
    }
  }
  printf("Consumer threads joined\n");
//Destroy pthread barrier
  if((pthread_barrier_destroy(&con_barrier)) != 0) {
    perror("pthread_barrier_destroy -c");
  }
  return 0;
}
//*******************************************************************
//main
//*******************************************************************
int main() {
  pid_t pid[2];
  int stat;
  

//Instantiate semaphores
  if((mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1)) == SEM_FAILED) {
    perror("sem_open mutex");
    exit(1);
  }
  if((semFull = sem_open(SEM_FULL, O_CREAT, 0666, BUFFER_SIZE)) == SEM_FAILED) {
    perror("sem_open semFull");
    sem_close(mutex);
    exit(1);
  }
  if((semEmpty = sem_open(SEM_EMPTY, O_CREAT, 0666, 0)) == SEM_FAILED) {
    perror("sem_open semEmpty");
    sem_close(mutex);
    sem_close(semEmpty);
    exit(1);
  }

//Instantiate shared memory
  if((shm_fd = shm_open(fileName, O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shm_open");
    sem_close(mutex);
    sem_close(semFull);
    sem_close(semEmpty);
    exit(1);
  }
  if((ftruncate(shm_fd, BUFFER_SIZE)) == -1) {
    perror("ftruncate");
    sem_close(mutex);
    sem_close(semEmpty);
    sem_close(semFull);
    shm_unlink(fileName);
    exit(1);
  }
  if((shmid = mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
	shm_fd, 0)) == MAP_FAILED) {
    perror("mmap");
    sem_close(mutex);
    sem_close(semEmpty);
    sem_close(semFull);
    shm_unlink(fileName);
    exit(1);
  }
//Generate producer child
  pid[0] = fork();
  if(pid[0] < 0) {
    perror("fork -p");
    sem_close(mutex);
    sem_close(semEmpty);
    sem_close(semFull);
    munmap(shmid, BUFFER_SIZE);
    shm_unlink(fileName);
    exit(1);
  }
  else if (pid[0] == 0) {
    Producer();
    return 0;
  }
  else {
    printf("Producer pid: %d\n", (int)pid[0]);
  }
//Generate consumer child
  pid[1] = fork();
  if(pid[1] < 0) {
    perror("fork -c");
    kill(pid[0], SIGKILL);
    sem_close(mutex);
    sem_close(semEmpty);
    sem_close(semFull);
    munmap(shmid, BUFFER_SIZE);
    shm_unlink(fileName);
    exit(1);
  }
  else if (pid[1] == 0) {
    Consumer();
    return 0;
  }
  else {
    printf("Comsumer pid: %d\n", (int)pid[1]);
  }
//Wait for children to die
  for (int i = 0; i < 2; i++) {
    pid_t cpid = waitpid(pid[i], &stat, 0);
    if (WIFEXITED(stat)) {
      printf("Child %d terminated with status: %d\n", cpid, WEXITSTATUS(stat));
    }
  }
//Destroy semaphores
  sem_unlink(SEM_MUTEX);
  sem_unlink(SEM_EMPTY);
  sem_unlink(SEM_FULL);
//Destroy shared memory
  if((munmap(shmid, BUFFER_SIZE)) == -1) {
    perror("munmap");
  }
  if((shm_unlink(fileName)) == -1) {
    perror("shm_unlink");
    exit(1);
  }

return 0;
}
