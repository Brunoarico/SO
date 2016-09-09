/******************************************************************************
 *  Nome:   Bruno Arico         8125459
 *          Gabriel Capella     8962078
 *
 *  Exercicio-programa de Sistemas Operacionais
 *
 *  Escalonadores de processo
 *
 ******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sched.h>

#define NUMBER_OF_QUEUES 4

pthread_mutex_t mutex;
float remaining_time = 0;
float Delta;
time_t start;
int debug;
FILE *saida;

typedef char* string;

typedef struct _process {
    char name[100];
    float t_begin;              //momento que chega
    float dt;                   // quanto dura
    float remaining;            // quanto resta
    float deadline;             // limite
    float quantum;              // classe de quantum para aquele processo
    int flag;                   // flag de ativo
    int id;                     // id do processo no routine
    struct _process *self;      // ja iniciado
    pthread_t tID;
} process;

typedef struct _cell {
   process proc; 
   struct _cell *prox;
} cell;

typedef cell* queue;

queue NewQueue () {
    cell *head;
    head = malloc (sizeof (cell));
    head->prox = head;
    head->proc.t_begin = -1;
    head->proc.remaining = -1;
    return head;
}

/*******************************************************************************************/

queue *NewMultipleQueues (int size) {
    queue *Qs;
    int i;
    Qs = malloc (size * sizeof (queue));
    /*printf("mallocquei o Qs em %d\n", size);*/
    for (i = 0; i < size; i++) {
        Qs[i] = NewQueue();
    }
    return Qs;
}

/*******************************************************************************************/

int is_Empty(queue Q) {
    if (Q->prox == Q)
        return 1;
    else
        return 0;
}

process Unqueue (queue Q) {
    process p;
    cell *this;
    if (!is_Empty (Q)){
        this = Q->prox;
        p = this->proc;
        Q->prox = this->prox;
        free (this);
        return p;
    } else {
        printf ("Desempilhou do vazio!\n");
        exit (1);
    }
}

process peek (queue Q) {
    process p;
    cell *this;
    if (!is_Empty (Q)) {
        this = Q->prox;
        p = this->proc;
        return p;
    } else {
        printf ("Olhou o vazio!\n");
        exit (1);
    }
}

queue to_Queue (process this, queue Q) {
    cell *new;
    new = malloc (sizeof (cell));
    new->prox = Q->prox;
    Q->prox = new;
    Q->proc = this;
    return new;
}
    
    
queue to_PQueue (process this, queue Q) {
    cell *new;
    cell *ant, *c;
    new = malloc (sizeof (cell));
    c = Q->prox;
    ant = Q;
    new->proc = this;
    while (1) {
        if(this.remaining < c->proc.remaining || c == Q) {
            ant->prox = new;
            new->prox = c;
            return Q;
        } else {
            ant = c;
            c=c->prox;
        }
    }
}


int Elements (queue Q) {
    queue aux;
    int i;
    aux = Q->prox;
    for (i = 0; aux != Q; i++, aux = aux->prox) {
        printf ("%s %d -> ", aux->proc.name, aux->proc.id);         
    }
    printf ("\n");
    return i;
}

int Elements2 (queue Q, process *routine) {
    queue aux;
    int i;
    aux = Q->prox;
    for (i = 0; aux != Q; i++, aux = aux->prox) {
        printf ("%s %d -> ", routine[aux->proc.id].name, aux->proc.id);         
    }
    printf ("\n");
    return i;
}

void *ThreadAdd (void *arg) {
    int time, i = 0;
    process P;
    P = *(process*) arg;
    pthread_mutex_lock (&mutex);

    time = clock();
    printf ("Comecando %s para durar %f\n", P.name, P.dt);
    while ((clock() - time) / CLOCKS_PER_SEC < P.dt){
        remaining_time = P.dt - (clock() - time) / CLOCKS_PER_SEC;
        i = i++ * 3*i + 4;
        //printf("%s %lu\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    }
    printf("Pronto %s em %lu segundos!\n", P.name, (clock() - time) / CLOCKS_PER_SEC);
    free(arg);

    pthread_mutex_unlock (&mutex);
    return NULL;
}

void FCFS (process *routine, int Nprocs) {
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

void *ThreadAdd2 (void *arg) {
    int i = 0;
    process P, *point;
    float begin, Tstart;
    P = *(process*) arg;
    point = (process*) arg;
    
    Tstart = begin = Delta;
    
    //printf("endereco da thread %s: %d\n", P.name, point);
    while (P.remaining > 0) {
        if (point->flag) {
            remaining_time = P.remaining = P.dt - (Delta - begin);
            i = i - 3 * i++;
            //printf("ligada thread %s falta %f\n",P.name, P.remaining);
        } else 
            begin = Delta; //printf("pausado %s falta %f flag = %d\n",P.name, P.remaining, point->flag);
    }
    
    printf ("%f s > Finalizado %s em %f segundos, tempo de parede %f. \n", Delta, P.name, P.dt, Delta - Tstart);
    free (arg);
    return (void*) 1;
}
process *initiate_thread (process p) {
    process *P;                                             
    P = malloc (sizeof (process));                               //aloca um novo P para passar para a thread
    *P = p;
    P->self = P;                                                //agora ele existe
    //printf("Criando: %s\n", P->name);
    pthread_create (&P->tID, NULL, ThreadAdd2, P);              //cria a thread
    remaining_time = P->remaining;                                     //atualiza o tempo de quem esta em execucão
    return P;                                                   //retorna um ponteiro para quem esta executando
}

void *ThreadAdd3 (void *arg) {
    int i = 0;
    time_t Tstart, Tstop;
    double tDelta;

    process P, *point;   
    P = *(process*) arg;
    point = (process*) arg;

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

/****************************************************************************************************/
process *initiate_thread2 (process p) {
    process *P;                                             //agora ele existe
    P = malloc (sizeof (process));                               //aloca um novo P para passar para a thread
    *P = p;
    printf ("Criando: %s \n", P->name);
    pthread_create (&P->tID, NULL, ThreadAdd3, P);              //cria a thread
    remaining_time = P->remaining;                                     //atualiza o tempo de quem esta em execucão
    return P;                                                   //retorna um ponteiro para quem esta executando
}
/****************************************************************************************************/


void pause_thread (process *p) {
    if (p->self != NULL) {
        p->flag = 0;
        //printf("Pausou thread %s\n", p->name);
    }
    else
        printf ("Pausou thread inexistente\n");
}

void continue_thread (process *p) {
    if (p->self != NULL){
        p->flag = 1;
        //printf("Continuando thread %s\n", p->name);
    }
    else
        printf ("Continuou thread inexistente\n");
}


int is_thread_exist (process *p) {
    if (p->self != NULL) return 1;
    else return 0;
}

void SRT (process *routine, int Nprocs) {
    queue Q, NQ;
    void *ret;
    int i, execs = 0;
    process *P, *EXE = NULL, aux;
    time_t stop;
    time (&start);
    Q = NewQueue ();
    
    while (!is_Empty (Q) || remaining_time || execs < Nprocs) {
        time (&stop);
        Delta = difftime (stop, start);                    // mede o tempo
        for (i = execs; i < Nprocs; i++) {
            if (routine[i].t_begin <= Delta) {                 // se algum processo chegar 
                execs++;
                if (routine[i].remaining < remaining_time) {        // se seu tempo for menor que o restante
                    if(EXE != NULL){                           // se tiver alguem rodando antes
                        pause_thread (EXE);                     // pausa ele
                        EXE->remaining = remaining_time;
                        Q = to_PQueue (*EXE, Q);                // e manda para a fila de volta
                        Elements(Q);
                    }
                    printf ("%f s > %s chegou dt = %f substituindo %s que tem %f para executar\n", Delta, routine[i].name, routine[i].remaining, EXE->name, remaining_time); 
                    EXE = initiate_thread (routine[i]);          // inicia o cara que tem urgencia
                } else {                                           //do contrario so manda para a fila
                    printf ("%f s > %s chegou e foi para a fila dt = %f\n", Delta, routine[i].name, routine[i].remaining);  
                    Q = to_PQueue (routine[i], Q);
                }
            }
        }
        
        if (!is_Empty (Q) && !remaining_time) {                                    // se houver alguem na fila
            EXE = NULL;
            aux = Unqueue (Q);                                 // tira da fila
            if (is_thread_exist (&aux)) {                      // se a thread ja existir
                printf ("%f s > Volta a executar %s que tem ainda %f s para terminar\n", Delta, aux.name, aux.remaining);
                EXE = aux.self;                                   // faz dele o que esta rodando
                continue_thread (EXE);                         // e manda ele continuar
            } else {
                printf ("%f s > Comeca a executar %s\n", Delta, aux.name);
                EXE = initiate_thread (aux);
            }
            Elements (Q);
        }
    }
    printf ("Fim!\n");
}

/****************************************************************************************
*                               MULTIPLAS FILAS                                         *
****************************************************************************************/

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

void MultiplasFIlas (process *routine, int Nprocs) {
    queue Q, NQ;
    int i = 0, a, execs = 0, *execflag, entraram = 0, contextswich = 0;
    process *P, *aux;
    time_t start, stop;
    time (&start);
    char *name;

    queue *Qs;

    float *quantuns = GerarVetorQuantuns(NUMBER_OF_QUEUES); /*quantuns sempre em potência de 2*/
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

        int filaatual = 0;
        int achou = 0;

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

int main (int argc, char **argv) {
    int i, j;
    FILE *trace;
    process routine[1000];
    unsigned long int time;

    debug = 0;

    if (argc > 4 && strcmp(argv[4], "d") == 0)
        debug = 1;

    trace = fopen (argv[2], "r");
    saida = fopen (argv[3], "w");
    
    if (trace == NULL){
        printf ("Problemas na abertura do arquivo\n");
        exit (1);
    }
    for(i = 0; fscanf (trace, "%f %s %f %f", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++) {
        //printf ("%f %s %f %f\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );
        routine[i].flag = 1;
        routine[i].remaining = routine[i].dt;
        routine[i].self = NULL;
        routine[i].id = i;
    }
    switch (atoi (argv[1])) {
        case 1:
            FCFS (routine, i);
            break;
        case 2:
            SRT (routine, i);
            break;
        case 3:
            MultiplasFIlas (routine, i);
            break;
        default:
            printf ("Escalonador invalido.\n");
    }

    pthread_exit (NULL);
    return 0;
}
