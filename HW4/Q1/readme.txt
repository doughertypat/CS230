CSci 230 (Fall 2019)
Programming Assignment 4

(Due in 2 weeks from the posting date)
(Hand-in should include the code files and the output of the programs)

1.	Multi-process programming to mimic the display of keyboard input on screen.

	Write a C program to read keystroke input from stdin and write to stdout. The 	program is required to produce 2 processes: a producer process and a consumer 	process. A circular queue is shared between the producer and the consumer. The 	producer process reads keystrokes from stdin and write these keystrokes to the shared 	queue. The consumer process reads keystrokes from the shared queue and write the 	keystrokes to the stdout. The main() procedure will finish when both the producer and 	the consumer have terminated. The producer will terminate when receiving a keystroke 	of character ‘*’ from stdin. Similarly, the consumer will terminate when read the 	character ‘*’ from the queue.

	A piece of sample code for implementing this program has been posted on blackboard. 	However, this piece of code does not work, and it only shows you the basic structure of 	the program. You have to reprogram this sample code to make it to be functional 	correctly. The execution of your program has to demonstrate that your program can 	handle long sequence of input keystrokes with the length of the sequence to 	significantly exceed the full size of the circular queue. For example, the length of the 	sequence of input keystrokes is twice the full size of the circular queue.

	Additional clarification:

	There are three separate components for implementing a solution for this question.
a)	The implementation of a shared circular queue between the producer and the consumer. The shared circular queue should be implemented by using the shared memory.
b)	The implementation of the synchronization on accessing the critical region (which is the shared circular queue) by using semaphore.
c)	The implementation of a source from which the producer fetches elements and push them into the shared circular queue. I have suggested you to use stdin as this source. Basically, a separate buffer is need to temporally hold up the input from stdin to allow the producer to fetch elements from this temporary buffer. This temporary buffer is totally different from the shared circular queue and can be implemented in different ways. The sole purpose of this component is to provide input to the producer.

I think the major confusions are originated from implementing component c). If it is hard for you to make keystrokes as the input source, then I can show you an alternative in which the content of an existing file is used as the input source, as shown below

#include <stdio.h>

int main(void)
{
    int i, i2;

    scanf("%d", &i); // read i from stdin

    // now change stdin to refer to a file instead of the keyboard
    freopen("someints.txt", "r", stdin);

    scanf("%d", &i2); // now this reads from the file "someints.txt"

    printf("Hello, world!\n"); // print to the screen

    // change stdout to go to a file instead of the terminal:
    freopen("output.txt", "w", stdout);

    printf("This goes to the file \"output.txt\"\n");

    // this is allowed on some systems--you can change the mode of a file:
    freopen(NULL, "wb", stdout); // change to "wb" instead of "w"

    return 0;
}

The examples I showed you serve to provide you with ideas rather than templates which can be easily made to work for you.

The key of the implementation is actually component b). It is suggested that you purposely produce the situations of “full queue” and “empty queue” to test the correctness of the synchronization between producer and consumer. You can consider to the sleep() calls to produce the situation of “full queue” by introducing a fast producer and a slow consumer. The situation of “empty queue” can be produced by introducing a slow producer and a fast consumer.
