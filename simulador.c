#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>     
#include <sys/stat.h>  
#include <semaphore.h>

sem_t mutex;
pthread_cond_t flag;

#define MAXPROC 100000
typedef char* string;

typedef struct _process {
    char name[100];
    int t_begin;
    int dt;
    int remaining;
    int deadline;
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
    return head;
}

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
    }
    else {
	printf ("Desempilhou do vazio!\n");
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
        if(this.remaining < c->proc.remaining ||c == Q) {
	    ant->prox = new;
	    new->prox = c;
	    return Q;
	}
	else{
	    ant = c;
	    c=c->prox;
	}
    }
}

int Elements (queue Q) {
    queue aux;
    int i;
    aux = Q->prox;
    for( i = 0; aux != Q; i++, aux = aux->prox){
	printf("%s -> ",aux->proc.name);	       
    }
    printf("\n");
    return i;
}



void *ThreadAdd (void *arg) {
    int time, i = 0;
    process P;
    sem_wait(&mutex);
    P= *(process*) arg;
    time = clock();
    printf("Começando %s para durar %d\n", P.name, P.dt);
    while((clock()-time)/CLOCKS_PER_SEC < P.dt){
	i++;
	//printf("%s %lu\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    }
    printf("Pronto %s em %lu segundos!\n", P.name, (clock()-time)/CLOCKS_PER_SEC);
    free(arg);
    sem_post(&mutex);
    return NULL;
}

void FCFS (process *routine, int Nprocs) {
    queue Q;
    int i, execs = 0;
    unsigned int t_start, delta;
    process *P;
    
    t_start = clock();
    Q = NewQueue();

    while (execs < Nprocs) {
	for (i = execs; i < Nprocs; i++) {
	    
	    delta = (clock() - t_start) / CLOCKS_PER_SEC;
	    if (routine[i].t_begin <= delta) {
		printf ("Ja chegou o %s! %d segundos\n",routine[i].name, delta);
		execs++;
	        Q = to_Queue (routine[i], Q);
	    }
	}
	
	if (!is_Empty(Q)) {
	    sem_wait(&mutex);
	    P = malloc( sizeof(process));
	    *P = Unqueue(Q);
	    pthread_create(&P->tID, NULL, ThreadAdd, P);
	    sem_post(&mutex);
	}
    }
}

void SRT (process *routine, int Nprocs) {
    queue Q;
    int i, execs = 0;
    unsigned int t_start, delta;
    process *P;
    
    t_start = clock();
    Q = NewQueue();

    while (execs < Nprocs) {
	for (i = execs; i < Nprocs; i++) {
	    delta = (clock() - t_start) / CLOCKS_PER_SEC;
	    if (routine[i].t_begin <= delta) {
		printf ("Ja chegou o %s! %d segundos\n",routine[i].name, delta);
		execs++;
		routine[i].remaining = routine[i].dt;
		Q = to_PQueue (routine[i], Q);
	    }
	}
    }    
}


int main() {
    int i, j;
    FILE *trace;
    process routine[1000];
    unsigned long int time;

    pthread_cond_init (&flag, NULL);

    if (sem_init(&mutex, 0, 1) == -1) {
      printf("Erro ao inicializar o semáforo :(\n");
      return(2);
    }

    trace = fopen ("trace.txt", "r");
    
    if (trace == NULL){
	printf ("Problemas na abertura do arquivo\n");
	exit (1);
    }
    for(i = 0; fscanf (trace, "%d %s %d %d", &routine[i].t_begin, routine[i].name, &routine[i].dt, &routine[i].deadline) != EOF; i++)
        printf("%d %s %d %d\n", routine[i].t_begin, routine[i].name, routine[i].dt, routine[i].deadline );
    
    SRT (routine, i);
    sem_destroy(&mutex);
    pthread_cond_destroy(&flag);
    pthread_exit(NULL);
    return 0;
}
