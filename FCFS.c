#include "FCFS.h"

pthread_mutex_t mutex;
time_t start;
float Delta;

void *ThreadAdd (void *arg) {
    int i = 0;
    time_t Tstart, Tstop;
    double tDelta;

    process P;
    P = *(process*) arg;

    //we can set one or more bits here, each one representing a single CPU
    cpu_set_t cpuset; 

    //the CPU we whant to use
    int cpu = 0;

    CPU_ZERO(&cpuset);       //clears the cpuset
    CPU_SET( cpu , &cpuset); //set CPU 0 on cpuset


    /*
    * cpu affinity for the calling thread 
    * first parameter is the pid, 0 = calling thread
    * second parameter is the size of your cpuset
    * third param is the cpuset in which your thread will be
    * placed. Each bit represents a CPU
    */
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    pthread_mutex_lock (&mutex);
    time (&Tstart);

    if (debug)
        fprintf(stderr, "%s usando a CPU %d\n", P.name, sched_getcpu());
    time (&Tstop);
    tDelta = difftime (Tstop, Tstart);

    //printf ("Comecando %s para durar %f\n", P.name, P.dt);
    while (tDelta < P.dt) {
        time (&Tstop);
        tDelta = difftime (Tstop, Tstart);
        i = i * 3*i + 4;
        //printf("%s %lu\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    }
    time (&Tstop);
    Delta = difftime (Tstop, start);
    if (debug) {
        fprintf(stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
        fprintf(stderr, "%f s > Acabou %s\n", Delta, P.name);
    }
    //printf("Pronto %s em %lu segundos!\n", P.name, (clock() - time) / CLOCKS_PER_SEC);
    free(arg);
    pthread_mutex_unlock (&mutex);
    return NULL;
}


void FCFS (process *routine, int Nprocs) {
    queue Q;
    int i = 0, execs = 0, contextswitch = 0;
    time_t stop;
    process *P;
    
    time (&start);
    Q = NewQueue ();
    while (execs < Nprocs || !is_Empty (Q)) {
        time (&stop);
        Delta = difftime (stop, start);

        while (i < Nprocs && routine[i].t_begin <= Delta) {
            if (debug)
                fprintf(stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
            Q = to_Queue (routine[i], Q);
            i++;
        }   
        /*for (i = execs; i < Nprocs; i++) {      
            delta = (clock () - t_start) / CLOCKS_PER_SEC;
            if (routine[i].t_begin <= delta) {
            printf ("Ja chegou o %s! %d segundos\n", routine[i].name, delta);
            execs++;
                Q = to_Queue (routine[i], Q);
            }
        }*/
        
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
