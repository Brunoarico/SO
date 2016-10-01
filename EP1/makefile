CFLAGS = -g -Wall -pthread

default : simulador terminal final

simulador : FCFS.o fila.o MQ.o SRT.o structs.o globals.o simulador.o cronometer.o
	$(CC) $(CFLAGS) FCFS.o fila.o MQ.o SRT.o simulador.o cronometer.o -o sim

terminal: ep1sh.o
	$(CC) $(CFLAGS) ep1sh.o -o ep1 -lreadline

ep1sh.o : ep1sh.c
	$(CC) $(CFLAGS) -c $^
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
cronometer.o : cronometer.c cronometer.h
	$(CC) $(CFLAGS) -c $^
simulador.o : simulador.c
	$(CC) $(CFLAGS) -c $^

final:
	$(RM) *.o
	find . -name "*.gch" -exec $(RM) -rf {} \;

clear:
	$(RM) ep1
	$(RM) sim
	$(RM) *.o
	find . -name "*.gch" -exec $(RM) -rf {} \;
	find . -name "*~" -exec $(RM) -rf {} \;
