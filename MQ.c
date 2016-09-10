#include "MQ.h"

pthread_mutex_t mutex;
float remaining_time = 0;
float Delta;

void *ThreadAdd3 (void *arg) {
    int i = 0;
    time_t Tstart, Tstop;
    double tDelta;

    process P, *point;   
    P = *(process*) arg;
    point = (process*) arg;

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

    /*if (P.dt == P.remaining)
        printf ("Comecando ");
    else
        printf ("Continuando ");*/
    /*printf("%s com tempo total %f, faltando %f, executando por %f\n", P.name, P.dt, P.remaining, P.quantum);*/
    if (debug)
        fprintf(stderr, "%s usando a CPU %d\n", P.name, sched_getcpu());
    time (&Tstop);
    tDelta = difftime (Tstop, Tstart);

    while (tDelta < P.quantum){
        time (&Tstop);
        tDelta = difftime (Tstop, Tstart);
        i++;
    }
    point->remaining = point->remaining - tDelta;
    /*printf("Executou %s em %f segundos! Tempo restante: %f\n", P.name, tDelta, point->remaining);*/
    if (debug)
        fprintf(stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
    free (arg);
    pthread_mutex_unlock (&mutex);
    return NULL;
}



process *initiate_thread2 (process p) {
    process *P;                                             //agora ele existe
    P = malloc (sizeof (process));                               //aloca um novo P para passar para a thread
    *P = p;
    printf ("Criando: %s \n", P->name);
    pthread_create (&P->tID, NULL, ThreadAdd3, P);              //cria a thread
    remaining_time = P->remaining;                                     //atualiza o tempo de quem esta em execucão
    return P;                                                   //retorna um ponteiro para quem esta executando
}

float *GerarVetorQuantuns (int size) {
    int i;
    float pot = 1;
    float *quantuns = malloc (size * sizeof (float));
    printf ("mallocquei o vetor de quantuns de tam %d\n", size);
    for (i = 0; i < size; i++) {
        pot = pot * 2;
        quantuns[i] = pot;
    }
    for (i = 0; i < size; i++)
        printf("%f ", quantuns[i]);
    printf("\n");
    return quantuns;
}

void LiberarVetorQuantuns (float *quantuns) {
    free (quantuns);
}

void MultiplasFIlas (process *routine, int Nprocs, int debug) {
    int i = 0, a, execs = 0, entraram = 0, contextswich = 0;
    process *P;
    time_t start, stop;
    int filaatual = 0;
    int achou = 0;
    queue *Qs;
    float *quantuns = GerarVetorQuantuns(NUMBER_OF_QUEUES); /*quantuns sempre em potência de 2*/
    time (&start);
    Qs = NewMultipleQueues (NUMBER_OF_QUEUES);
    printf("%d %d\n", execs, Nprocs);
    while (execs < Nprocs) { /*não terminei de executar todos*/
        time (&stop); /*checa tempo*/
        Delta = difftime (stop, start);
        /*existe(m) processo(s) da rotina para entrar em f0*/
        while (i < Nprocs && routine[i].t_begin <= Delta) {
            /*coloca novos processos em f0*/
            if (debug)
                fprintf (stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
            printf ("%f s > mandando %s %d para a fila inicial\n", Delta, routine[i].name, routine[i].id);
            Qs[0] = to_Queue (routine[i], Qs[0]);
            for (a = 0; a < NUMBER_OF_QUEUES; a++) {
                printf ("Fila %d ", a);
                Elements2(Qs[a], routine);
            }
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
                P = malloc (sizeof (process));
                *P = Unqueue (Qs[filaatual]);
                //strcpy (name, P->name);
                P->quantum = quantuns[filaatual];
                printf ("%f s > comeca a rodar %s %d por %f e tem ate %f para executar\n", Delta, routine[P->id].name, P->id, quantuns[filaatual], P->t_begin + P->deadline);
                pthread_create (&P->tID, NULL, ThreadAdd3, P);
                pthread_join(P->tID, NULL);
                time (&stop);
                Delta = difftime (stop, start);
                printf("%f s > Tempo restante de %s %d: %f\n", Delta, routine[P->id].name, P->id, P->remaining);

                 /*se não terminar passar pra próxima fila */
                if (P->remaining > 0) {
                    //ocorre troca de contexto
                    contextswich++;
                    if (filaatual != (NUMBER_OF_QUEUES - 1)) {
                        filaatual++;
                        P->quantum = quantuns[filaatual];
                        printf ("%f s > Ja chegou o %s %d! na fila %d\n", Delta, routine[P->id].name, P->id, filaatual);
                        Qs[filaatual] = to_Queue (*P, Qs[filaatual]);
                    } else {
                            /* ta na última fila (fila circular) */
                        printf ("%f s > Ja chegou o %s %d! na fila %d\n", Delta, routine[P->id].name, P->id, filaatual);
                        Qs[filaatual] = to_Queue (*P, Qs[filaatual]);
                    }
                    for (a = 0; a < NUMBER_OF_QUEUES; a++) {
                        printf ("Fila %d ", a);
                        Elements2 (Qs[a], routine);
                    }
                } else {
                    if (Delta > (P->t_begin + P->deadline)) {
                        printf ("Excedeu deadline %s %f %f \n", routine[P->id].name, Delta, (P->t_begin + P->deadline));

                    }
                    printf("%f s > Acabou %s %d\n", Delta, routine[P->id].name, P->id);
                    if (debug)
                        fprintf (stderr, "%f s > Finalizou: %s %f %f\n", Delta, routine[P->id].name, Delta, Delta - P->t_begin);
                    fprintf (saida, "%s %f %f\n", routine[P->id].name, Delta, Delta - P->t_begin);
                    for (a = 0; a < NUMBER_OF_QUEUES; a++) {
                        printf ("Fila %d ", a);
                        Elements2 (Qs[a], routine);
                    }
                    execs++;
                }
            } else {
                filaatual++;
            }
        }
    }
    if (debug)
        fprintf(stderr, "%d\n", contextswich);
    fprintf(saida, "%d\n", contextswich);
}
