#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/types.h>
#include<errno.h>

#define BUFFER_SIZE 16
#define READ_END 0
#define WRITE_END 1

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int initsem(key_t key, int nsems)  /* key from ftok() */
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0) { /* we got it first */
        sb.sem_op = 1; sb.sem_flg = 0;
        arg.val = 1;

        printf("press return\n"); getchar();

        for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) { 
            /* do a semop() to "free" the semaphores. */
            /* this sets the sem_otime field, as needed below. */
            if (semop(semid, &sb, 1) == -1) {
                int e = errno;
                semctl(semid, 0, IPC_RMID); /* clean up */
                errno = e;
                return -1; /* error, check errno */
            }
        }

    } else if (errno == EEXIST) { /* someone else got it first */
        int ready = 0;

        semid = semget(key, nsems, 0); /* get the id */
        if (semid < 0) return semid; /* error, check errno */

        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i = 0; i < 10 && !ready; i++) {
            semctl(semid, nsems-1, IPC_STAT, arg);
            if (arg.buf->sem_otime != 0) {
                ready = 1;
            } else {
                sleep(1);
            }
        }
        if (!ready) {
            errno = ETIME;
            return -1;
        }
    } else {
        return semid; /* error, check errno */
    }

    return semid;
}

int main()
{

	key_t SHMKEY;
	key_t SEMKEY;
	int semid;
	struct sembuf sbp;
	struct sembuf sbc;
	unsigned short semval;
	
	sbp.sem_num = 0;
	sbp.sem_op = -1;
	sbp.sem_flg = IPC_NOWAIT;

	sbc.sem_num = 0;
	sbc.sem_op = -1;
	sbc.sem_flg = 0;
	
	if ((SEMKEY = ftok("CS230_HW4v2.c", 'A')) == -1) {
		perror("ftok");
		exit(1);
	}

	if ((semid = initsem(SEMKEY,3)) == -1) { //Make 3 semaphores, mutex, full, empty
		perror("initsem");
		exit(1);
	} 
	//FIXME: Set full to 15, empty to 0 and leave mutex alone

	//Make producer child
	int pid=fork();
	if(pid<0) {
		perror("Fork failed!\n"); }
	else if(pid==0) {
		printf("The producer is here.\n");		
		char write_msg[2];
		int r = 0;
		int w = 0;
		char temp_buf[100];
		while(1) {
			//temp_buf[w] = getchar();	//read char into internal buffer
			//w = (w+1)%100;		//increment write location

			sbp.sem_op = -1;
			//semval = semctl(semid, 0, GETVAL);	//used for error checking
			//printf("%d\n", semval);		//print semval
			if (semop(semid, &sbp, 1) == -1) {
				if (errno == EAGAIN) {
					errno = 0;
					continue;
				}
				perror("Producer semop lock");
				exit(1);
			}
			printf ("Producer has lock\n");
			


			sbp.sem_op = 1;
			if (semop(semid, &sbp, 1) == -1) {
				perror("Producer semop release");
				exit(1);
			}
			printf ("Producer released lock\n");
			sleep(1);		//FIXME: Remove after debugging!		
		}			
		return 0;
	}
	else {
		printf("Producer pid: %d\n", pid); }
	
	//Make consumer child
	pid=fork();
	if(pid<0) {
		perror("Fork 2 failed!\n"); }
	else if(pid==0) {
		printf("The consumer is here.\n");
		while (1) {		//FIXME: make sure to add \r when \n is received
			sbc.sem_op = -1;
			printf("Consumer attempting to lock\n");
			if (semop(semid, &sbc, 1) == -1) {
				perror("Consumer semop lock");
				exit(1);
			}
			if (errno == EAGAIN) {
				printf("Consumer cannot lock\n");
				errno = 0;
				continue;
			}
			printf("Consumer has lock\n");
			sleep(1);
			sbc.sem_op = 1;
			if (semop(semid, &sbc, 1) == -1) {
				perror("Consumer semop unlock");
				exit(1);
			}
			printf ("Consumer released lock\n");
			sleep(1);
		}
		return 0;
	}
	else {
		printf("Consumer pid: %d\n", pid); }

	while(wait(NULL)!=-1);
	printf("\nAll children are dead. Exiting Parent.\n");
	return 0;
}			
