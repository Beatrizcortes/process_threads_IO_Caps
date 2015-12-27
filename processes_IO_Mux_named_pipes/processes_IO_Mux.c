#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "processes_strings.h"

int ioMux (int readend, int writeend1, int writeend2, char name[]){
//IO function. Reads from keyboard, then multiplexes and sends through pipe
	int r, a;
	char buff[SIZE];
	char half1[SIZE_HALF];
	char half2[SIZE_HALF];

	printf("Type lines of text (Ctrl+D to finish):\n");
	for (;;){
	//goes on until interupted by signal
		if(fgets (buff, SIZE, stdin)){
		//read from keyboard
			
			a = strlen(buff)+ 1;//take into account EOF
			printf("%s: separating %d bytes\n", name, a);
			split(buff, half1, half2);//split data in two
			//send in pipe at different times. If error, abort 
			if(write(writeend1, half1, 1+strlen(half1)) < 0)
				break;
			if(write(writeend2, half2, 1+strlen(half2)) < 0)
				break;
		}
		else{
			close(writeend1); //close write streams
			close(writeend2);
		}

		r = read (readend, buff, SIZE);

		if (r<0) //abort when error happens
			break;
		else if (r == 0) //pipe closed = ok
			return 0;

		fputs (buff, stdout); //show me return of child and keep going
	}

	perror("io error: "); //broke the loop upon error
	return -1;
}

int main (int argc, char *argv[]) {
	if(argc!=5) {
	//check for arguments
		printf("Four arguments expected. Can't execute.\n");
		printf("Usage: ./io_mux [read pipe] [write pipe] [write pipe] [process name]\n");
		return 1;
	}
	//argv 4 will be name of process. We don't really need it here
	int writeend1 = open(argv[2],O_WRONLY);
	int writeend2 = open(argv[3],O_WRONLY);	
	int readend = open(argv[1],O_RDONLY);
	return ioMux(readend, writeend1, writeend2, argv[3]);

}
