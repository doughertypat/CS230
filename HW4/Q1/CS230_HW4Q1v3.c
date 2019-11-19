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
#define SEM_MUTEX 0
#define SEM_FULL 1
#define SEM_EMPTY 2

union semun {
        int val;
        struct semid_ds *buf;
        ushort *array;
};

int initsem(key_t key, int nsems)  /* key from ftok() - A special thanks to Beej's Guides for this charm */
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

int stdin_select(void)
{
	fd_set rd;
	struct timeval tv={0};
	int ret;
	FD_ZERO(&rd);
	FD_SET(STDIN_FILENO, &rd);
	if(ret=select(1, &rd, NULL, NULL, &tv)==-1) {
		errno = 0;
	}
	return ret;
}

int main()
{
	int fd[2];
	
	key_t SHMKEY;
        key_t SEMKEY;
        int semid;
        struct sembuf full_write;
	struct sembuf full_read;
	//struct sembuf empty_write;
        //struct sembuf empty_read;
	//struct sembuf sbp;
	//struct sembuf sbc;
        unsigned short semval;
	union semun arg;
	
	//Initialize sembuf structures
        //sbp.sem_num = 0;
        //sbp.sem_op = -1;
        //sbp.sem_flg = IPC_NOWAIT;
        //sbc.sem_num = 0;
        //sbc.sem_op = -1;
        //sbc.sem_flg = 0;
	full_write.sem_num = SEM_FULL;
	full_write.sem_op = -1;
	full_write.sem_flg = IPC_NOWAIT;
	full_read.sem_num = SEM_FULL;
	full_read.sem_op = 1;
	full_read.sem_flg = 0;
	//empty_write.sem_num = 2;
	//empty_write.sem_op = 1;
	//empty_write.sem_flg = 0;
	//empty_read.sem_num = 2;
	//empty_read.sem_op = -1;
	//empty_read.sem_flg = 0;

	//Make shared memory
	if(pipe(fd) == -1) {
		perror("shm");
		return 1;
	}
	//Make semaphore key
        if ((SEMKEY = ftok("CS230_HW4v2.c", 'A')) == -1) {
                perror("ftok");
                exit(1);
        }
	//Make semaphore set
        if ((semid = initsem(SEMKEY,3)) == -1) { //Make 3 semaphores, mutex, full, empty
                perror("initsem");
                exit(1);
	}
	//Set sem full to 16 and empty to 0
	arg.val = BUFFER_SIZE;
	if ((semctl(semid, SEM_FULL, SETVAL, arg)) == -1) {
		perror("semctl");
		exit(1);
	}
	//arg.val = 0;
	//if ((semctl(semid, SEM_EMPTY, SETVAL, arg)) == -1) {
	//	perror("semctl");
	//	exit(1);
	//}
	
	printf("This producer-consumer program mimics typing in the terminal\n");
	printf("You must type * to exit. Escape characters will not work\n");	

	//Make producer child
	int pid=fork();
	if(pid<0) {
		perror("Fork failed!\n"); }
	else if(pid==0) {
		printf("The producer is started.\n");
		close(fd[READ_END]);
		char write_msg[1];
		char temp_buf[100]; //circular queue to hold chars temporarily
		int r = 0;		//read index
		int w = 0;		//write index
		system("/bin/stty raw -echo icrnl"); //modify terminal behavior
		while(1){
		    //int semval = semctl(semid, SEM_FULL, GETVAL);  //for troubleshooting
		    if(stdin_select() || w==r){ //if chars waiting in stdin or temp_buf empty
		    temp_buf[w] = getchar();
		    w = (1+w)%100;
		    }
		    //printf("%d\n\r", semval);
		    if (semop(semid, &full_write, 1) == -1) { //try to decrement SEM_FULL
			if (errno == EAGAIN) { //Nonblocking sem - if shm full go back start
			    errno = 0;		//must reset errno 
			    continue;
			}
			perror("semop-full_write"); //catch semop failure other than EAGAIN
			exit(1);
		    }
		    //sleep(1); //Uncomment sleep statement to emulate slow producer
		    write(fd[WRITE_END], &temp_buf[r], 1);
		    //printf("%d\n\r", semval);
		    if (temp_buf[r] == '*') {   //exit producer when * is typed
			system("/bin/stty sane"); //restore terminal behavior
			printf("\nKilling producer\n");
			break;
		    } 	
		    r = (1+r)%100;
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
		close(fd[WRITE_END]);
		char read_msg;
		while(1) {
		    sleep(1);	//Uncomment sleep statement to emulate slow consumer
		    read(fd[READ_END], &read_msg, 1);
		    if (read_msg == '*'){ //exit consumer when * is typed
		    	system("/bin/stty sane");
			printf("\nKilling consumer\n");
			break; }
		    if (read_msg == '\n') {
		        printf("\r");
		    }
		    printf("%c", read_msg);
		    fflush(stdout);
		    if (semop(semid, &full_read, 1) == -1) {
			perror("semop-full_read");
			exit(1);
		    } 
		}
		return 0;
	}
	else {
		printf("Consumer pid: %d\n", pid); }

	while(wait(NULL)!=-1);
	system("/bin/stty sane"); //return term to normal
	printf("\nAll children are dead. Exiting Parent.\n");
	return 0;
}			
