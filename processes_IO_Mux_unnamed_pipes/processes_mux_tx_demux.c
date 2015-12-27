#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "processes_strings.h"
#include "processes_caps.h"
#include "processes_demux.h"
#include "processes_IO_Mux.h"

int main(){
	int io[2], a[2], b1[2], b2[2], demux1[2], demux2[2], r;

	if (pipe(io) || pipe(a) || pipe(b1) || pipe(b2) || pipe(demux1) || pipe(demux2)){
	//open all of the pipes
		perror("pipe failed: ");
		return -1;
	}

	switch(fork())
	{
		case -1: perror("fork failed!: ");//error case
			return -2;//no break needed as last statement is return
		case 0: close(io[0]);//child case
			close(a[1]);
			close(a[0]);
			close(b1[0]);
			close(b1[1]);
			close(b2[0]);
			close(b2[1]);
			close(demux1[1]);
			close(demux2[1]);
			return demux(demux1[0], demux2[0], io[1]); //1st child is demux
		default: break;
	}

	switch(fork())
	{
		case -1: perror("fork failed!: ");
			return -3;
		case 0: close(io[0]);//child case
			close(io[1]);
			close(a[1]);
			close(b1[0]);
			close(b1[1]);
			close(b2[0]);
			close(b2[1]);
			close(demux1[0]);
			close(demux2[1]);
			close(demux2[0]);
			return caps(a[0], demux1[1], "A");//2nd child is capsA
		default: break;
	}

	switch(fork())
	{
		case -1: perror("fork failed!: ");
			return -4;
		case 0: close(io[0]);//child case
			close(io[1]);
			close(a[1]);
			close(a[0]);
			close(b1[1]);
			close(b2[0]);
			close(demux1[1]);
			close(demux1[0]);
			close(demux2[1]);
			close(demux2[0]);
			return caps(b1[0], b2[1], "B1");//3rd child is capsB1
		default: break;
	}

	switch(fork())
	{
		case -1: perror("fork failed!: ");
			return -5;
		case 0: close(io[0]);//child case
			close(io[1]);
			close(a[1]);
			close(b1[0]);
			close(b1[1]);
			close(a[0]);
			close(b2[1]);
			close(demux1[0]);
			close(demux1[1]);
			close(demux2[0]);
			return caps(b2[0], demux2[1], "B2");//4th child is capsB2
		default: close(demux1[1]);//parent becomes IO
			close(io[1]);
			close(a[0]);
			close(b1[0]);
			close(b2[0]);
			close(b2[1]);
			close(demux1[0]);
			close(demux2[1]);
			close(demux2[0]);
			r = ioMux(io[0], a[1], b1[1]);
			wait (NULL);
			wait (NULL);
			wait (NULL);
			wait (NULL);
			return r;
	}
}
