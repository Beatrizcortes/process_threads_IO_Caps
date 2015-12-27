#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "processes_strings.h"

int caps (int readend, int writeend, char name[]){
//Random caps conversion process. Converts from what it reads in pipe

	int i, r, a;
	char buff[SIZE];

	srand ((unsigned)getpid() ^(unsigned)time(NULL));
	
	for (;;){
	//loops until it's interrupted by a signal
		r = read(readend, buff, SIZE); //get string from pipe

		if(r<0)//if error, abort
			break;
		else if (r==0){
			//if pipe closed, close yours and finish
			close(writeend);
			return 0;
		}
	
		a = strlen(buff) + 1;//take into account EOF
		printf("%s: transmitting %d bytes\n", name, a);

		for (i=0; buff[i]; i++) //randomly make caps for all string
			if (rand() > RAND_MAX/2)
				if (buff[i] >= 'a' && buff[i] <= 'z')
					buff[i] -=('a' - 'A');
					//and send back!
		if (write(writeend, buff, 1+strlen(buff)) < 0)
			break;
	}

	perror("caps failed: "); //came here from loop on error
	return -1;
}

int main (int argc, char *argv[]) {
	if(argc!=4) {
	//check for arguments
		printf("Three arguments expected. Can't execute\n");
		printf("Usage: ./tx [read pipe] [write pipe] [process name]\n");
		return 1;
	}
	int readend = open(argv[1],O_RDONLY);
	int writeend = open(argv[2], O_WRONLY);
	return caps(readend, writeend, argv[3]);
}
