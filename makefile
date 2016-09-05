ep1 : ep1sh.o
	cc -o ep1 ep1sh.o -lreadline

ep1sh.o : ep1sh.c

sim : simulador.o
	cc -o sim simulador.o -pthread 

simulador.o : simulador.c