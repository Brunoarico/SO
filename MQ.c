/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Nicolas Nogueira    9277541
 *
 *  Sistemas Operacionais MAC0422
 *
 *****************************************************************************/

#include "MQ.h"

pthread_mutex_t mutex;
float remaining_time = 0;
float Delta;
int dead

void *ThreadAdd3 (void *arg) {
    int i = 0;
    tempo Tstart, Tstop;
    double tDelta;
    process P, *point;   
    cpu_set_t cpuset; 
    int cpu = 0;
    P = *(process*) arg;
    point = (process*) arg;
    CPU_ZERO (&cpuset);      
    CPU_SET (cpu , &cpuset); 

    sched_setaffinity (0, sizeof (cpuset), &cpuset);

    pthread_mutex_lock (&mutex);
    clock_gettime(CLOCK_MONOTONIC, &Tstart);

    if (debug)
        fprintf (stderr, "%s usando a CPU %d\n", P.name, sched_getcpu());
    clock_gettime(CLOCK_MONOTONIC, &Tstop);
    tDelta = diff (Tstop, Tstart);

    while (tDelta < P.quantum){
        clock_gettime(CLOCK_MONOTONIC, &Tstop);
        tDelta = diff (Tstop, Tstart);
        i++;
    }
    point->remaining = point->remaining - tDelta;
    if (debug)
        fprintf (stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
 
    pthread_mutex_unlock (&mutex);
    return NULL;
}

float *GerarVetorQuantuns (int size) {
    int i;
    float pot = 1;
    float *quantuns = malloc (size * sizeof (float));
    for (i = 0; i < size; i++) {
        pot = pot * 2;
        quantuns[i] = pot;
    }
    
    return quantuns;
}

void LiberarVetorQuantuns (float *quantuns) {
    free (quantuns);
}

void MultiplasFIlas (process *routine, int Nprocs) {
    int i = 0, a, execs = 0, entraram = 0, contextswitch = 0;
    process *P, aux;
    tempo start, stop;
    int filaatual = 0;
    int achou = 0;
    queue *Qs;
    float *quantuns = GerarVetorQuantuns(NUMBER_OF_QUEUES); /*quantuns sempre em potência de 2*/
    clock_gettime(CLOCK_MONOTONIC, &start);
    Qs = NewMultipleQueues (NUMBER_OF_QUEUES);
 
    while (execs < Nprocs) { /*não terminei de executar todos*/
        clock_gettime(CLOCK_MONOTONIC, &stop); /*checa tempo*/
        Delta = diff (stop, start);
        /*existe(m) processo(s) da rotina para entrar em f0*/
        while (i < Nprocs && routine[i].t_begin <= Delta) {
            /*coloca novos processos em f0*/
            if (debug)
                fprintf (stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
            Qs[0] = to_Queue (routine[i], Qs[0]);
            i++;
            entraram++;
        }

        /* procura quem é o prox processo a executar */

        achou = 0;
        filaatual = 0;

        while (entraram != 0 && !achou) {
            if (!is_Empty(Qs[filaatual])) { /* assumindo que NUMBER_OF_QUEUES > 0 */
                achou = 1;
                /* faz unqueue e executa aquele quantum */
                aux = Unqueue (Qs[filaatual]);
		P = &aux;
                P->quantum = quantuns[filaatual];
                pthread_create (&P->tID, NULL, ThreadAdd3, P);
                pthread_join(P->tID, NULL);
                clock_gettime(CLOCK_MONOTONIC, &stop);
                Delta = diff (stop, start);
                 /*se não terminar passar pra próxima fila */
                if (P->remaining > 0) {
                    //ocorre troca de contexto
                    contextswitch++;
                    if (filaatual != (NUMBER_OF_QUEUES - 1)) {
                        filaatual++;
                        P->quantum = quantuns[filaatual];
                        Qs[filaatual] = to_Queue (*P, Qs[filaatual]);
                    } else {
                            /* ta na última fila (fila circular) */
                        Qs[filaatual] = to_Queue (*P, Qs[filaatual]);
                    }
                } else {
                    if (debug)
                        fprintf (stderr, "%f s > Finalizou: %s %f %f\n", Delta, routine[P->id].name, Delta, Delta - P->t_begin);
                    fprintf (saida, "%s %f %f\n", routine[P->id].name, Delta, Delta - P->t_begin);
                    execs++;
                }
            } else {
                filaatual++;
            }
        }
    }
    if (debug)
        fprintf(stderr, "%d\n", contextswitch);
    fprintf(saida, "%d\n", contextswitch);
    fprintf (
}
