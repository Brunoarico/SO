/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#include "FCFS.h"

pthread_mutex_t mutex;
tempo start={0,0};
float Delta;

void *ThreadAdd (void *arg) {
    int i = 0;
    tempo Tstart, Tstop;
    double tDelta;
    process P;
    cpu_set_t cpuset; 
    int cpu = 0;

    CPU_ZERO(&cpuset);      
    CPU_SET( cpu , &cpuset);
    P = *(process*) arg;

    
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    pthread_mutex_lock (&mutex);
    clock_gettime(CLOCK_MONOTONIC, &Tstart);

    if (debug)
        fprintf(stderr, "%s usando a CPU %d\n", P.name, sched_getcpu());
    clock_gettime(CLOCK_MONOTONIC, &Tstop);
    tDelta = diff (Tstop, Tstart);

    while (tDelta < P.dt) {
	clock_gettime(CLOCK_MONOTONIC, &Tstop);
	tDelta = diff (Tstop, Tstart);
        i = i * 3*i + 4;

    }
    clock_gettime(CLOCK_MONOTONIC, &Tstop);
    Delta = diff (Tstop, start);
    if (debug) {
        fprintf(stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
        fprintf(stderr, "%f s > Acabou %s\n", Delta, P.name);
    }
    free(arg);
    pthread_mutex_unlock (&mutex);
    return NULL;
}


void FCFS (process *routine, int Nprocs) {
    queue Q;
    int i = 0, execs = 0, contextswitch = 0;
    tempo stop;
    process *P;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    Q = NewQueue ();
    while (execs < Nprocs || !is_Empty (Q)) {
        clock_gettime(CLOCK_MONOTONIC, &stop);
        Delta = diff (stop, start);

        while (i < Nprocs && routine[i].t_begin <= Delta) {
            if (debug)
                fprintf(stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
            Q = to_Queue (routine[i], Q);
            i++;
        }   
      
        if (!is_Empty (Q)) {
            P = malloc (sizeof (process));
            *P = Unqueue (Q);
            pthread_create (&P->tID, NULL, ThreadAdd, P);
            execs++;
        }
    }
    pthread_join (P->tID, NULL);
    if (debug)
        fprintf(stderr, "%d\n", contextswitch);
}
