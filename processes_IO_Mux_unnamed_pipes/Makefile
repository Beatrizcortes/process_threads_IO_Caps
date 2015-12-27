all: processes_mux_tx_demux

processes_mux_tx_demux: processes_mux_tx_demux.o processes_demux.o processes_caps.o processes_strings.o processes_IO_Mux.o
	gcc -g -Wall -o processes_mux_tx_demux processes_mux_tx_demux.o processes_demux.o processes_caps.o processes_strings.o processes_IO_Mux.o

processes_mux_tx_demux.o: processes_mux_tx_demux.c processes_strings.h processes_caps.h processes_demux.h processes_IO_Mux.h
	gcc -g -Wall -c -o processes_mux_tx_demux.o processes_mux_tx_demux.c

processes_demux.o: processes_demux.c processes_strings.h processes_demux.h
	gcc -g -Wall -c -o processes_demux.o processes_demux.c

processes_caps.o: processes_caps.c processes_strings.h processes_caps.h
	gcc -g -Wall -c -o processes_caps.o processes_caps.c

processes_strings.o: processes_strings.c processes_strings.h
	gcc -g -Wall -c -o processes_strings.o processes_strings.c

processes_IO_Mux.o: processes_IO_Mux.c processes_IO_Mux.h processes_strings.h
	gcc -g -Wall -c -o processes_IO_Mux.o processes_IO_Mux.c
clean:
	rm -f processes_mux_tx_demux
	rm -f *.o
