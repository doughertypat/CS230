#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>

#define BUFFER_SIZE 16
#define READ_END 0
#define WRITE_END 1

int main()
{
	int fd[2];
	if(pipe(fd) == -1) {
		printf("Pipe creation failed.\n");
		return 1;
	}

	//Make producer child
	int pid=fork();
	if(pid<0) {
		perror("Fork failed!\n"); }
	else if(pid==0) {
		printf("The producer is started.\n");
		close(fd[READ_END]);
		char write_msg[2];
		while(1){
		    system("/bin/stty raw -echo icrnl"); //modify terminal behavior
		    write_msg[0] = getchar();
		    if (write_msg[0] == '\n') { //if NL also add CR
			write_msg[1]= '\r'; }
		    else {
			write_msg[1] = '\0';} //fill second byte with \0
		    write(fd[WRITE_END], write_msg, 2);
		    system("/bin/stty cooked echo"); //return term to normal
		    if (write_msg[0] == '*') {
			break; } //exit producer when * is typed
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
			read(fd[READ_END], &read_msg, 1);
			if (read_msg == '*'){ //exit consumer when * is typed
				break; }
			printf("%c", read_msg);
			fflush(stdout);
		}
		return 0;
	}
	else {
		printf("Consumer pid: %d\n", pid); }

	while(wait(NULL)!=-1);
	printf("\nAll children are dead. Exiting Parent.\n");
	return 0;
}			
