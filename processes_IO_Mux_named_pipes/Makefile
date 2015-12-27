all: demux io_mux tx
	mknod T0 p
	mknod T1 p
	mknod T2 p
	mknod T3 p
	mknod T4 p
	mknod T5 p

io_mux: processes_strings.o processes_IO_Mux.o
	gcc -g -Wall -o io_mux processes_strings.o processes_IO_Mux.o

demux: processes_demux.o processes_strings.o
	gcc -g -Wall -o demux processes_strings.o processes_demux.o

tx: processes_strings.o processes_caps.o
	gcc -g -Wall -o tx processes_strings.o processes_caps.o

processes_demux.o: processes_demux.c processes_strings.h
	gcc -g -Wall -c -o processes_demux.o processes_demux.c

processes_caps.o: processes_caps.c processes_strings.h
	gcc -g -Wall -c -o processes_caps.o processes_caps.c

processes_strings.o: processes_strings.c processes_strings.h
	gcc -g -Wall -c -o processes_strings.o processes_strings.c

processes_IO_Mux.o: processes_IO_Mux.c processes_strings.h
	gcc -g -Wall -c -o processes_IO_Mux.o processes_IO_Mux.c
clean:
	rm -f tx
	rm -f demux
	rm -f io_mux
	rm -f *.o
	rm -f T?
