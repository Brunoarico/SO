CFLAGS = -g -Wall -pthread

default : FCFS.o fila.o MQ.o SRT.o structs.o globals.o simulador.o
	$(CC) $(CFLAGS) *.o -o sim

FCFS.o : FCFS.c FCFS.h
	$(CC) $(CFLAGS) -c $^
fila.o : fila.c fila.h
	$(CC) $(CFLAGS) -c $^
MQ.o : MQ.c MQ.h
	$(CC) $(CFLAGS) -c $^
SRT.o : SRT.c SRT.h
	$(CC) $(CFLAGS) -c $^
structs.o : structs.h 
	$(CC) $(CFLAGS) -c $^
globals.o : globals.h
	$(CC) $(CFLAGS) -c $^
simulador.o : simulador.c
	$(CC) $(CFLAGS) -c $^

ep1 : ep1sh.o
	cc -o ep1 ep1sh.o -lreadline

ep1sh.o : ep1sh.c
	$(CC) $(CFLAGS) -c $^
clear:
	$(RM) sim
	$(RM) *.o
	find . -name "*.gch" -exec $(RM) -rf {} \;
	find . -name "*~" -exec $(RM) -rf {} \;
