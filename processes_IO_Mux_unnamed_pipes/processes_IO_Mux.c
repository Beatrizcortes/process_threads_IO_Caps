#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "processes_IO_Mux.h"
#include "processes_strings.h"

int ioMux (int readend, int writeend1, int writeend2){
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
			printf("Mux: separating %d bytes\n", a);
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
