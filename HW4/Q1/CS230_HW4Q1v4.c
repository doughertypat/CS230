#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<semaphore.h>
#include<fcntl.h>

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
	//Semaphore variables and structures
	key_t SEMKEY;
        int semid;
        struct sembuf full_write;
	struct sembuf full_read;
	struct sembuf empty_write;
        struct sembuf empty_read;
	struct sembuf mutex_lock;
	struct sembuf mutex_unlock;
        unsigned short semval;
	union semun arg;

	//Shared memory variables
	void *shmid;
	char* filePath = "HW4shm";
	int shm_fd;
	
	
	//Initialize sembuf structures
        mutex_lock.sem_num = SEM_MUTEX;
        mutex_lock.sem_op = -1;
        mutex_lock.sem_flg = 0;
        mutex_unlock.sem_num = SEM_MUTEX;
        mutex_unlock.sem_op = 1;
        mutex_unlock.sem_flg = 0;
	full_write.sem_num = SEM_FULL;
	full_write.sem_op = -1;
	full_write.sem_flg = IPC_NOWAIT;
	full_read.sem_num = SEM_FULL;
	full_read.sem_op = 1;
	full_read.sem_flg = 0;
	empty_write.sem_num = SEM_EMPTY;
	empty_write.sem_op = 1;
	empty_write.sem_flg = 0;
	empty_read.sem_num = SEM_EMPTY;
	empty_read.sem_op = -1;
	empty_read.sem_flg = 0;

	//Make semaphore and shared memory keys
        if ((SEMKEY = ftok("CS230_HW4Q1v4.c", 'A')) == -1) {
                perror("ftok");
                exit(1);
        }
	
	//Create shared memory
	if((shm_fd = shm_open(filePath, O_CREAT | O_RDWR, 0666)) == -1) {
		perror("shm_open");
		exit(1);
	}
	//Truncate shared memory to BUFFER_SIZE
	if((ftruncate(shm_fd, BUFFER_SIZE)) == -1) {
		perror("ftruncate");
		exit(1);
	}
	//Map shared memory
	if((shmid = mmap(0, BUFFER_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	char *str = (char *)shmid; //cast shmid to char type for pointer arithmatic operations	
	
	//Make semaphore set //FIXME: Change semnum to argv*2+1 
        if ((semid = initsem(SEMKEY,3)) == -1) { //Make 3 semaphores, mutex, full, empty
                perror("initsem");
                exit(1);
	}
	//Set sem full to 16 and empty to 0 //FIXME: for loop to modify all, SEM_FULL and SEM_EMPTY will no longer be valid
	arg.val = BUFFER_SIZE;
	if ((semctl(semid, SEM_FULL, SETVAL, arg)) == -1) {
		perror("semctl");
		exit(1);
	}
	arg.val = 0;
	if ((semctl(semid, SEM_EMPTY, SETVAL, arg)) == -1) {
		perror("semctl");
		exit(1);
	}
	
	//Make producer child
	int pid=fork();
	if(pid<0) {
		perror("Fork failed!\n"); }
	else if(pid==0) {
		printf("The producer is started.\n");
		char temp_buf[100];	
		char write_msg[1];
		int r = 0;		//read index
		int w = 0;		//write index
		int shmW = 0;
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
                            errno = 0;          //must reset errno 
                            continue;
                        }
                        perror("semop-full_write"); //catch semop failure other than EAGAIN
                        exit(1);
                    }
		    //Enter critical region - decrement SEM_MUTEX
		    if (semop(semid, &mutex_lock, 1) == -1) {  
                        perror("semop-mutex_lock"); 
                        exit(1);
                    }
                    //sleep(1); //Uncomment sleep statement to emulate slow producer
                    //Write to circular buffer
		    str[shmW]=temp_buf[r];
		    //Exit critical region - increment mutex
		    if (semop(semid, &mutex_unlock, 1) == -1) {
			perror("semop-metex_unlock");
			exit(1);
		    }
		    //Increment SEM_EMPTY to indicate char added to queue
		    if (semop(semid, &empty_write, 1) == -1) {
			perror("semop-metex_unlock");
			exit(1);
		    }
		    //Increment shm write index
		    shmW = (1+shmW)%16;
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
		char read_msg;
		int shmR = 0;
		while(1) {
		    sleep(1);	//Uncomment sleep statement to emulate slow consumer
		    //Check SEM_EMPTY to see if item has been added to queue
		    if (semop(semid, &empty_read, 1) == -1) {
			perror("semop-empty_read");
			exit(1);
		    }
		    //Check SEM_MUTEX to see if critical region is occupied - if free lock mutex
		    if (semop(semid, &mutex_lock, 1) == -1) { 
                        perror("semop-mutex_lock"); //catch semop failure
                        exit(1);
                    }
		    
		    //Enter critical region - read from shared circular queue
		    read_msg = str[shmR];
		    
		    //Exit critical region - unlock mutex
		    if (semop(semid, &mutex_unlock, 1) == -1) {
			perror("semop-mutex_unlock");
			exit(1);
		    }
		    //Increment SEM_FULL to indicate char read from queue
		    if (semop(semid, &full_read, 1) == -1) {
			perror("semop-full_read");
			exit(1);
		    }
		    //Increment shm read index	
		    shmR = (1+shmR)%16;
		    //If read char is * exit consumer
		    if (read_msg == '*'){ 
		    	system("/bin/stty sane");
			printf("\nKilling consumer\n");
			break;
		    }
		    //If \n was read then also print \r
		    if (read_msg == '\n') {
		        printf("\r");
		    }
		    //Print read char and flush stdout
		    printf("%c", read_msg);
		    fflush(stdout);
		     
		}
		return 0;
	}
	else {
		printf("Consumer pid: %d\n", pid); }

	while(wait(NULL)!=-1);
	system("/bin/stty sane"); //return term to normal
	printf("\nAll children are dead. Exiting Parent.\n");
	//Remove memory mapping
	if(munmap(shmid, BUFFER_SIZE) == -1) {
		perror("munmap");
	}
	//Unlink shared memory
	if (shm_unlink(filePath) == -1) {
		perror("shm_unlink");
	}
	//Remove semaphore set
	if (semctl(semid, 0, IPC_RMID) == -1) {
		perror("semctl");
	}
	return 0;
}			
