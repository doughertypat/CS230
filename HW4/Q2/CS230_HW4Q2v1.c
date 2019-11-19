#include<unistd.h> //ftruncate(), select()
#include<stdio.h> //getchar()
#include<stdlib.h> //exit()
#include<sys/wait.h> //wait()
#include<sys/mman.h> //mmap(), munmap()
#include<sys/ipc.h> //shmget(), semctl()
#include<sys/sem.h> //semctl()
#include<sys/shm.h> //shmget()
#include<sys/types.h> //ftruncate(), semctl(), select()
#include<errno.h> //errno
#include<fcntl.h> //O_CREAT O_RDWR
#include<ctype.h> //isdigit()

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

//Function to create and check semaphore set - Thank you to Beej's Guide
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

//Function to check stdin for input using select
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


int main(int argc, char** argv)
{
	//Get command-line arguments
	if (argc == 1 || argc > 2) {
		printf("Improper usage - <program name> <int # of consumers>");
		exit(1);
	}
	if (!isdigit(*argv[1])) {
		printf("Improper usage - argument must be an integer (such as 3)");
		exit(1);
	}
	printf("%s",argv[1]);
	int numC = atoi(argv[1]);
	
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
        if ((SEMKEY = ftok("CS230_HW4Q2v1.c", 'A')) == -1) {
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
	//Make semaphore set - We need 1 full and 1 empty semaphore for each consumer, plus 1 mutex (hence numC*2 + 1) 
        if ((semid = initsem(SEMKEY,(1+numC*2))) == -1) { 
                perror("initsem");
                exit(1);
	}
	//Set sem full to 16 and empty to 0 
	arg.val = BUFFER_SIZE;
	for(int i = 1; i < (1+numC*2); i+=2) {
	    if ((semctl(semid, i, SETVAL, arg)) == -1) {
		perror("semctl1");
		exit(1);
	    }
	}
	arg.val = 0;
	for(int i = 2; i < (1+numC*2); i+=2) {
	    if ((semctl(semid, i, SETVAL, arg)) == -1) {
		perror("semctl2");
		exit(1);
	    }
	}
	//Make producer child
	int pid=fork();
	if(pid<0) {
		perror("Fork failed!\n"); }
	else if(pid==0) {
		printf("The producer is started.\n");
		char temp_buf[100];	//internal buffer
		int r = 0;		//read index
		int w = 0;		//write index
		int shmW = 0;
		char c;
		FILE *file = fopen("test.txt", "r");
		//system("/bin/stty raw -echo icrnl"); //modify terminal behavior
		while(temp_buf[w] != EOF) {
                    temp_buf[w] = fgetc(file);
                    w = (1+w)%100;
                    for(int i=1; i< (1+numC*2); i+=2){
		    full_write.sem_num = i;
		    //Check if any consumer has not read the 16th byte
		    if (semop(semid, &full_write, 1) == -1) { //try to decrement SEM_FULL
                        if (errno == EAGAIN) { //Nonblocking sem - if shm full go back start
                            //errno = 0;          //must reset errno 
                            continue;
                        }
                        perror("semop-full_write"); //catch semop failure other than EAGAIN
                        exit(1);
                    }
		    }
		    if (errno == EAGAIN) {
			errno = 0;
			continue;
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
		    for (int i=2; i<(1+numC*2); i+=2){
		    empty_write.sem_num = i;
		    if (semop(semid, &empty_write, 1) == -1) {
			perror("semop-metex_unlock");
			exit(1);
		    }
		    }
		    //Increment shm write index
		    shmW = (1+shmW)%16;
		    //Increment read index for internal buffer
                    r = (1+r)%100;
		    
		   
		   
                }
		printf("\nKilling producer\n");
		return 0;
        }		
	else {
		printf("Producer pid: %d\n", pid); }
	
	//Make consumer child
	for (int i=1; i<=numC; i++) {
	pid=fork();
	if(pid<0) {
		perror("Consumer fork failed!\n"); }
	else if(pid==0) {
		printf("Consumer %d is here.\n", i);
		//Update sembuf structures
		empty_read.sem_num = 2*i;
		full_read.sem_num = (2*i)-1; 
		//Instantiate consumer variables
		char read_msg;
		int shmR = 0;
		char filename[10];
		if((sprintf(filename,"test%d.txt", i)) == -1) {
			perror("sprintf");
		}
		printf("New file: %s\n", filename);
		FILE *newfile = fopen(filename, "w");
		if (newfile == NULL) {
		    perror("newfile");
		}
		//Begin consumer magic (aka make some files)
		while(1) {
		    //sleep(1);	//Uncomment sleep statement to emulate slow consumer
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
		    //If EOF is passed then kill consumer. File is done.
		    if (read_msg == EOF) {
			break;
		    }
		    //Copy received char to newfile
		    fprintf(newfile, "%c", read_msg);
		     
		}
		printf("Killing consumer %d\n", i);
		fclose(newfile);
		return 0;
	}
	
	else {
		printf("Consumer pid: %d\n", pid); }
	}
	while(wait(NULL)!=-1); //Wait for children to die
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
