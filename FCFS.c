#include "FCFS.h"

static pthread_mutex_t mutex;
static float Delta;
static float remaining_time = 0;

void *ThreadAdd (void *arg) {
    int time, i = 0;
    process P;
    P = *(process*) arg;
    pthread_mutex_lock (&mutex);

    time = clock();
    printf ("Comecando %s para durar %f\n", P.name, P.dt);
    while ((clock() - time) / CLOCKS_PER_SEC < P.dt){
        remaining_time = P.dt - (clock() - time) / CLOCKS_PER_SEC;
        i = i * 3*i + 4;
        //printf("%s %lu\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    }
    printf("Pronto %s em %lu segundos!\n", P.name, (clock() - time) / CLOCKS_PER_SEC);
    free(arg);

    pthread_mutex_unlock (&mutex);
    return NULL;
}


void FCFS (process *routine, int Nprocs, int debug) {
    queue Q;
    int i, execs = 0;
    unsigned int t_start, delta;
    process *P;
    
    t_start = clock ();
    Q = NewQueue ();
    while (execs < Nprocs || !is_Empty (Q)) {
   
        for (i = execs; i < Nprocs; i++) {      
            delta = (clock () - t_start) / CLOCKS_PER_SEC;
            if (routine[i].t_begin <= delta) {
            printf ("Ja chegou o %s! %d segundos\n", routine[i].name, delta);
            execs++;
                Q = to_Queue (routine[i], Q);
            }
        }
        
        if (!is_Empty (Q)) {
            P = malloc (sizeof (process));
            *P = Unqueue (Q);
            pthread_create (&P->tID, NULL, ThreadAdd, P);
        }
    }
}
