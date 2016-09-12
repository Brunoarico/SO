#include "SRT.h"

static float Delta;
static float Remaining_time = 0;
static tempo start={0,0};
static int dead = 0;
pthread_mutex_t mutex;


void *ThreadAdd2 (void *arg) {
    int i = 1;
    process P, *point;
    float begin, Tstart;
    cpu_set_t cpuset; 
    int cpu = 0;
    int pausado = 1;
    
    P = *(process*) arg;
    point = (process*) arg;
    Tstart = begin = Delta;
    
    CPU_ZERO (&cpuset);       //clears the cpuset
    CPU_SET (cpu , &cpuset); //set CPU 0 on cpuset
    sched_setaffinity (0, sizeof (cpuset), &cpuset);

    while (P.remaining >= 0) {
      	//printf("Remaining_time %f\n", Remaining_time);
        if (point->flag) {
            if (pausado && debug) {
                fprintf (stderr, "%s usando a CPU %d\n", P.name, sched_getcpu());
                pausado = 0;
            }
	    pthread_mutex_lock (&mutex);
            Remaining_time = P.remaining = P.dt - (Delta - begin);
	    pthread_mutex_unlock (&mutex);
            i = i - 3 * i;
	    
	    //printf("%s rodando\n", P.name);
        }
	else {
            if (debug && !pausado) {
                pausado = 1;
                fprintf (stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
            }
	    //printf("%s parado\n", P.name);
	    
            begin = Delta;
        }
    }
    pthread_mutex_lock (&mutex);
    Remaining_time = 0;
    pthread_mutex_unlock (&mutex);
    if (debug) {
        fprintf (stderr, "%s liberou a CPU %d\n", P.name, sched_getcpu());
        fprintf (stderr, "%f s > Finalizou: %s %f %f\n", Delta, P.name, Delta, Delta - Tstart);
	if(Delta-Tstart > P.deadline)
	    dead++;
    }
    //printf ("%f s > Finalizado %s em %f segundos, tempo de parede %f. \n", Delta, P.name, P.dt, Delta - Tstart);
    free (arg);
    return NULL;
}

process *initiate_thread (process p) {
    process *P;                                             
    P = malloc (sizeof (process));                               //aloca um novo P para passar para a thread
    *P = p;
    P->self = P;                                                //agora ele existe
    //printf("Criando: %s\n", P->name);
    pthread_create (&P->tID, NULL, ThreadAdd2, P);              //cria a thread
    Remaining_time = P->remaining;                                     //atualiza o tempo de quem esta em execucão
    return P;                                                   //retorna um ponteiro para quem esta executando
}

void pause_thread (process *p) {
    if (p->self != NULL) {
        p->flag = 0;
        printf("Pausou thread %s\n", p->name);
    }
    else
        printf ("Pausou thread inexistente\n");
}

void continue_thread (process *p) {
    if (p->self != NULL){
        p->flag = 1;
        printf("Continuando thread %s\n", p->name);
    }
    else
        printf ("Continuou thread inexistente\n");
}


int is_thread_exist (process *p) {
    if (p->self != NULL) return 1;
    else return 0;
}

void SRT (process *routine, int Nprocs) {
    queue Q;
    int i, execs = 0, contextswitch = 0;
    process *EXE = NULL, aux;
    tempo stop={0,0};
    clock_gettime(CLOCK_MONOTONIC, &start);
    Q = NewQueue ();
    
    while (!is_Empty (Q) || Remaining_time || execs < Nprocs) {
	pthread_mutex_lock (&mutex);
        clock_gettime(CLOCK_MONOTONIC, &stop);
        Delta = diff(stop, start);                    // mede o tempo
        for (i = execs; i < Nprocs; i++) {
            if (routine[i].t_begin <= Delta) {                 // se algum processo chegar 
                execs++;
                if (routine[i].remaining < Remaining_time) {        // se seu tempo for menor que o restante
		            if (debug)
			             fprintf (stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
		    
                    if (EXE != NULL){                           // se tiver alguem rodando antes
                        pause_thread (EXE);                     // pausa ele
                        EXE->remaining = Remaining_time;
                        Q = to_PQueue (*EXE, Q);                // e manda para a fila de volta
                    }
		    
                    contextswitch++;
                    //printf ("%f s > %s chegou dt = %f substituindo %s que tem %f para executar\n", Delta, routine[i].name, routine[i].remaining, EXE->name, Remaining_time); 
                    EXE = initiate_thread (routine[i]);          // inicia o cara que tem urgencia
                } else {                                           //do contrario so manda para a fila
                    //printf ("%f s > %s chegou e foi para a fila dt = %f\n", Delta, routine[i].name, routine[i].remaining);  
                    Q = to_PQueue (routine[i], Q);
                }
            }
        }
       
        if (!is_Empty (Q) && !Remaining_time) {                                    // se houver alguem na fila
            EXE = NULL;
            aux = Unqueue (Q);                                 // tira da fila
            if (is_thread_exist (&aux)) {                      // se a thread ja existir
		Remaining_time = aux.remaining;
                printf ("%f s > Volta a executar %s que tem ainda %f s para terminar\n", Delta, aux.name, aux.remaining);
                EXE = aux.self;                                   // faz dele o que esta rodando
                continue_thread (EXE);                         // e manda ele continuar
            } else {
                printf ("%f s > Comeca a executar %s\n", Delta, aux.name);
                EXE = initiate_thread (aux);
            }
            Elements (Q);
        }
	pthread_mutex_unlock (&mutex);
    }
    if (debug)
        fprintf(stderr, "%d\n", contextswitch);
    
    fprintf(saida, "%d\n", contextswitch);
    printf ("Fim!\n");
    fprintf(debugfile, "%d %d\n", contextswitch, dead);
}
