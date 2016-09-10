#include "SRT.h"

static float Delta;
static float remaining_time = 0;
static time_t start;

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
    queue Q;
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
		    
		    if (debug)
			fprintf (stderr, "%f s > Chegou %s no sistema: %f %s %f %f\n", Delta, routine[i].name, routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline);
		    
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
