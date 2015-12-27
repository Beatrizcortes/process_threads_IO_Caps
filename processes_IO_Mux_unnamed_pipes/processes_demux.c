#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "processes_strings.h"
#include "processes_demux.h"

int demux (int readend1, int readend2, int writeend){
//Recieves data from pipes, rejoins both halves and displays result
	char all[SIZE];
	char half1[SIZE_HALF];
	char half2[SIZE_HALF];
	int r;

	for(;;){
	//Loops until interrupted by a signal
		r = read(readend1, half1, SIZE_HALF);//get first half
		
		if(r<0)//if error, abort
			break;
		else if (r==0){
			//if pipe closed, finish and close your end
			close (writeend);
			return 0;
		}
		
		r = read(readend2, half2, SIZE_HALF); //get second half

		if(r<0)//if error, abort
			break;
		else if (r==0){
			//if pipe closed, finish and close your end
			close (writeend);
			return 0;
		}
	
		int a = strlen(half1)+ 1;//take into account EOF
		int b = strlen(half2)+ 1;	
		printf("Demux: joining %d and %d bytes\n", a, b);
		concatenate(all, half1, half2);

		if (write(writeend, all, 1+strlen(all)) < 0)
			break;
	}

	perror("demux error: "); //broke the loop upon error
	return -1;
}
